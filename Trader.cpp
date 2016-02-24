        /* 
 * File:   Trader.cpp
 * Author: volchnik
 * 
 * Created on February 23, 2015, 11:56 AM
 */

#include "Trader.h"
#include "Operator/OperatorAdd.h"
#include "Operator/OperatorSeries.h"

Trader::Trader(std::shared_ptr<Series> stock, std::shared_ptr<Series> allow,
        long timeoutAfterDeal, long update_level_interval, long maxPosition, double diffOffsetPersent, double stop_loss_percent) :
    tradeStock_(stock), tradeAllowSignal_(allow),
    timeoutAfterDeal_(timeoutAfterDeal), update_level_interval_(update_level_interval),
    maxPosition_(maxPosition), timeoutAfterDealCooldown_(0), diffOffsetPersent_(diffOffsetPersent), stop_loss_percent_(stop_loss_percent) {
}

Trader::~Trader() {
}

std::tuple<double, Series, Series, Series, Series, Series> Trader::Trade(std::shared_ptr<Operator> signal_strategy) const {
    OperatorAdd resultOperatorAdd(std::make_shared<OperatorSeries>(std::make_shared<Series>(tradeStock_->GenerateZeroBaseSeries())),
                                  signal_strategy);
    std::shared_ptr<Operator> resultOperator = resultOperatorAdd.perform();
    return this->Trade(*dynamic_cast<OperatorSeries*>(resultOperator.get())->getSeries());
}

std::tuple<double, Series, Series, Series, Series, Series> Trader::Trade(const Series& trade_signal) const {
    struct tm time_struct;
    operationType signal;
    
    Series trade_position = trade_signal * 0.0;
    Series trade_account = trade_signal * 0.0;
    Series trade_account_moment = trade_signal * 0.0;
    
    Series trade_limit_buy(trade_signal);
    Series trade_limit_sell(trade_signal);
    
    int current_position = 0;
    double current_account = 0.0;
    double current_profit = 0.0;
    
    double limit_buy_level = 0.0;
    double limit_sell_level = 0.0;
    long update_level_cooldown_seconds = 0;
    long timeout_after_deal_seconds = 0;
    
    for (auto& datetime_series : trade_signal.datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));

        for (auto& sample : datetime_series.second.series_) {
            float trade_stock_value = this->tradeStock_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
            if (current_position != 0 && (current_account + current_position * trade_stock_value - current_profit) < -1 * maxPosition_ * trade_stock_value * stop_loss_percent_) {
                makeDeal(current_position > 0 ? operationType::SELL : operationType::BUY, trade_stock_value, current_account, current_position, current_profit);
            } else if ((signal = GetCurrentSignal(datetime_series.second.begin_interval + offset, pday_of_year,
                                           sample.value, trade_stock_value, current_position, trade_limit_buy,
                                           trade_limit_sell, limit_buy_level, limit_sell_level, update_level_cooldown_seconds, timeout_after_deal_seconds))
                != operationType::STAY) {
                makeDeal(signal, trade_stock_value, current_account, current_position, current_profit);
            }
            trade_position.SetValue(datetime_series.second.begin_interval + offset, current_position, pday_of_year);
            trade_account.SetValue(datetime_series.second.begin_interval + offset, current_profit, pday_of_year);

            trade_account_moment.SetValue(datetime_series.second.begin_interval + offset, (current_position != 0) ? (current_account + current_position * trade_stock_value - current_profit) : 0, pday_of_year);
            
            offset++;
        }
    }

    return std::make_tuple(current_account, trade_position, trade_account, trade_limit_buy, trade_limit_sell, trade_account_moment);
}

Trader::operationType Trader::GetCurrentSignal(long datetime, std::shared_ptr<DayOfTheYear> pday_of_year, float value, float trade_stock_value, int current_position, Series& trade_limit_buy, Series& trade_limit_sell, double& limit_buy_level, double& limit_sell_level, long& update_level_cooldown_seconds, long& timeout_after_deal_seconds) const {

    if (update_level_cooldown_seconds == 0) {
        update_level_cooldown_seconds = this->update_level_interval_;
        // Обновляем лимитные уровни
        float trade_allow_level = this->tradeAllowSignal_->GetValue(datetime, pday_of_year);
        
        float currenPositionBuy = current_position + ((current_position >= 0) ?
            (1.0 / trade_allow_level) :
            (2.0 - 1.0 / trade_allow_level));
        if (value >= (currenPositionBuy - 1.0)) {
            limit_buy_level = trade_stock_value -
                    this->diffOffsetPersent_ * trade_stock_value *
                    max((double)currenPositionBuy - value, 0.0);
            trade_limit_buy.SetValue(datetime, limit_buy_level, pday_of_year);
        } else {
            limit_buy_level = -numeric_limits<double>::max();
        }
        
        double currenPositionSell = current_position - ((current_position <= 0) ?
            (1.0 / trade_allow_level) :
            (2.0 - 1.0 / trade_allow_level));
        if (value <= (currenPositionSell + 1.0)) {
            limit_sell_level = trade_stock_value +
                    this->diffOffsetPersent_ * trade_stock_value *
                    max(value - currenPositionSell, 0.0);
            trade_limit_sell.SetValue(datetime, limit_sell_level, pday_of_year);
        } else {
            limit_sell_level = numeric_limits<double>::max();
        }
    } else {
        if (limit_buy_level > -numeric_limits<double>::max()) {
            trade_limit_buy.SetValue(datetime, limit_buy_level, pday_of_year);
        }
        if (limit_sell_level < numeric_limits<double>::max()) {
            trade_limit_sell.SetValue(datetime, limit_sell_level, pday_of_year);
        }
        update_level_cooldown_seconds--;
    }
    
    if (timeout_after_deal_seconds) {
        timeout_after_deal_seconds--;
    }
    
    if ((current_position < 0 || maxPosition_ > labs(current_position)) &&
            timeout_after_deal_seconds == 0 &&
            trade_stock_value <= limit_buy_level) {
        update_level_cooldown_seconds = 0;
        timeout_after_deal_seconds = this->timeoutAfterDeal_;
        return operationType::BUY;
    } else if ((current_position > 0 ||maxPosition_ > labs(current_position)) &&
            timeout_after_deal_seconds == 0 &&
            trade_stock_value >= limit_sell_level) {
        update_level_cooldown_seconds = 0;
        timeout_after_deal_seconds = this->timeoutAfterDeal_;
        return operationType::SELL;
    }

    return operationType::STAY;
}

void Trader::makeDeal(operationType signal, float trade_stock_value, double& current_account, int& current_position, double& current_profit) const {
    if (signal != operationType::STAY) {
        current_account += ((signal == operationType::BUY) ? -1 : 1) * trade_stock_value;
        current_position += ((signal == operationType::BUY) ? 1 : -1);
        if (current_position == 0) {
            current_profit = current_account;
        }
    }
}

double Trader::CalculateTradeFitness(const Series& trade_position, const Series& trade_account, const Series& trade_account_moment) {
    struct tm time_struct;
    unsigned long deal_count = 0;
    double deal_ammount = 0.0;
    double max_moment_loss_quadratic_ammount = 0.0;
    unsigned long deal_sum_time = 0;
    
    for (auto& datetime_series : trade_position.datetime_map_) {
        unsigned long offset = 0;
        unsigned long deal_start_offset = 0;
        bool deal_started = false;
        float max_loss_moment = 0.0;
        std::shared_ptr<DayOfTheYear> pday_of_year =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        
        for (auto& sample : datetime_series.second.series_) {
            float trade_position_current = trade_position.GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
            if (!deal_started && trade_position_current != 0) {
                deal_started = true;
                max_loss_moment = 0.0;
                deal_start_offset = offset;
            } else if (deal_started && trade_position_current != 0) {
                float trade_account_moment_current = trade_account_moment.GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
                if (max_loss_moment > trade_account_moment_current) {
                    max_loss_moment = trade_account_moment_current;
                }
            } else if (deal_started && trade_position_current == 0) {
                deal_started = false;
                double amount_diff = trade_account.GetValue(datetime_series.second.begin_interval + offset, pday_of_year) -
                    trade_account.GetValue(datetime_series.second.begin_interval + offset - 1, pday_of_year);
                deal_ammount += (amount_diff > 0) ? amount_diff : (1.0 * amount_diff) - 10;
                max_moment_loss_quadratic_ammount += pow(0.001 * max_loss_moment, 2);
                deal_sum_time += (offset - deal_start_offset);
                deal_count++;
            }
            offset++;
        }
    }

    return (deal_count > 0 && deal_sum_time > 0 && max_moment_loss_quadratic_ammount > 0) ? ((double)deal_ammount / (deal_sum_time / (double)deal_count)) : 0;
}