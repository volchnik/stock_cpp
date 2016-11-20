        /* 
 * File:   Trader.cpp
 * Author: volchnik
 * 
 * Created on February 23, 2015, 11:56 AM
 */

#include "Trader.h"
#include "Operator/OperatorAdd.h"
#include "Operator/OperatorMultiply.h"
#include "Operator/OperatorSeries.h"

Trader::Trader(std::shared_ptr<Series> stock, std::shared_ptr<Series> stock_min,
               std::shared_ptr<Series> stock_max, std::shared_ptr<Series> stock_si, std::shared_ptr<Series> trade_multiplier,
               std::shared_ptr<Series> allow,
               long timeoutAfterDeal, long update_level_interval, long maxPosition,
               double diffOffset, double stop_loss_percent, unsigned long start_trade_offset) :
    tradeStock_(stock), tradeStockMin_(stock_min), tradeStockMax_(stock_max),
    trade_si_(stock_si), trade_multiplier_(trade_multiplier),
    tradeAllowSignal_(allow),
    timeoutAfterDeal_(timeoutAfterDeal),
    update_level_interval_(update_level_interval),
    maxPosition_(maxPosition),
    timeoutAfterDealCooldown_(0),
    diffOffset_(diffOffset),
    stop_loss_percent_(stop_loss_percent),
    start_trade_offset_(start_trade_offset) {
}

Trader::~Trader() {
}

std::tuple<double, Series, Series, Series, Series, Series, Series> Trader::Trade(std::shared_ptr<Operator> signal_strategy) const {
    OperatorAdd resultOperatorAdd(std::make_shared<OperatorSeries>(std::make_shared<Series>(tradeStock_->GenerateZeroBaseSeries())),
                                  signal_strategy);
    
    OperatorMultiply resultOperatorMultiply(std::make_shared<OperatorSeries>(this->trade_multiplier_),
                                  std::make_shared<OperatorAdd>(resultOperatorAdd));
    std::shared_ptr<Operator> resultOperator = resultOperatorMultiply.perform();
    //trade_multiplier_
    return this->Trade(*dynamic_cast<OperatorSeries*>(resultOperator.get())->getSeries());
}

std::tuple<double, Series, Series, Series, Series, Series, Series> Trader::Trade(const Series& trade_signal) const {
    struct tm time_struct;
    operationType signal;
    
    Series trade_position = trade_signal * 0.0;
    Series trade_account = trade_signal * 0.0;
    Series trade_account_moment = trade_signal * 0.0;
    
    Series trade_limit_buy(*this->tradeStock_);
    trade_limit_buy.SetName("trade_limit_buy");
    Series trade_limit_sell(*this->tradeStock_);
    trade_limit_sell.SetName("trade_limit_sell");
    
    int current_position = 0;
    double current_account = 0.0;
    double current_profit = 0.0;
    
    double limit_buy_level = -numeric_limits<double>::max();
    double limit_sell_level = numeric_limits<double>::max();
    double limit_buy_level_fix_deal = -numeric_limits<double>::max();
    double limit_sell_level_fix_deal = numeric_limits<double>::max();
    
    long update_level_cooldown_seconds = 0;
    long timeout_after_deal_seconds = 0;
    unsigned long begin_trade_signal = trade_signal.datetime_map_.begin()->second.begin_interval;
    
    for (auto& datetime_series : trade_signal.datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));

        for (auto& sample : datetime_series.second.series_) {
            
            if (datetime_series.second.begin_interval + offset - begin_trade_signal < this->start_trade_offset_) {
                continue;
            }
            float trade_stock_value = this->tradeStock_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
            float trade_stock_min_value = this->tradeStockMin_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
            float trade_stock_max_value = this->tradeStockMax_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
            float trade_si = this->trade_si_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
            /*if (current_position != 0 && (current_account + current_position * trade_stock_value - current_profit) < -1 * maxPosition_ * trade_stock_value * stop_loss_percent_) {
                makeDeal(current_position > 0 ? operationType::SELL : operationType::BUY, current_account, current_position, current_profit, trade_stock_value, trade_stock_value, trade_si, pday_of_year);
            } else*/ if ((signal = GetCurrentSignal(datetime_series.second.begin_interval + offset, pday_of_year,
                                                  sample.value, trade_stock_value, trade_stock_min_value, trade_stock_max_value,
                                                  current_position, trade_limit_buy,
                                                  trade_limit_sell, limit_buy_level, limit_sell_level,
                                                  limit_buy_level_fix_deal, limit_sell_level_fix_deal, update_level_cooldown_seconds,
                                                  timeout_after_deal_seconds))
                != operationType::STAY) {
                makeDeal(signal, current_account, current_position, current_profit,
                         limit_buy_level_fix_deal, limit_sell_level_fix_deal, trade_si, pday_of_year);
            }
            trade_position.SetValue(datetime_series.second.begin_interval + offset, current_position, pday_of_year);
            trade_account.SetValue(datetime_series.second.begin_interval + offset, current_profit, pday_of_year);

            trade_account_moment.SetValue(datetime_series.second.begin_interval + offset, (current_position != 0) ? (current_account + current_position * trade_stock_value - current_profit) : 0, pday_of_year);
            
            offset++;
        }
    }

    return std::make_tuple(current_account, trade_position, trade_account, trade_limit_buy, trade_limit_sell, trade_account_moment, trade_signal);
}

Trader::operationType Trader::GetCurrentSignal(long datetime, std::shared_ptr<DayOfTheYear> pday_of_year, float value,
                                               float trade_stock_value, float trade_stock_min_value, float trade_stock_max_value,
                                               int current_position, Series& trade_limit_buy,
                                               Series& trade_limit_sell, double& limit_buy_level, double& limit_sell_level,
                                               double& limit_buy_level_fix_deal, double& limit_sell_level_fix_deal,
                                               long& update_level_cooldown_seconds, long& timeout_after_deal_seconds) const {
    
    Trader::operationType return_operation = operationType::STAY;

    if ((current_position < 0 || maxPosition_ > labs(current_position)) &&
        timeout_after_deal_seconds == 0 &&
        trade_stock_min_value <= limit_buy_level) {
        
        update_level_cooldown_seconds = 0;
        timeout_after_deal_seconds = this->timeoutAfterDeal_;
        return_operation = operationType::BUY;
        limit_buy_level_fix_deal = limit_buy_level;
        
    } else if ((current_position > 0 ||maxPosition_ > labs(current_position)) &&
               timeout_after_deal_seconds == 0 &&
               trade_stock_max_value >= limit_sell_level) {
        
        update_level_cooldown_seconds = 0;
        timeout_after_deal_seconds = this->timeoutAfterDeal_;
        return_operation = operationType::SELL;
        limit_sell_level_fix_deal = limit_sell_level;
    }
    
    if (update_level_cooldown_seconds == 0) {
        // Обновляем лимитные уровни
        float trade_allow_level = this->tradeAllowSignal_->GetValue(datetime, pday_of_year);
        
        float currentPositionBuy = (current_position + ((current_position >= 0) ?
                                                        (1.0 / trade_allow_level) :
                                                        (2.0 - 1.0 / trade_allow_level)));
        
        /*if (currentPositionBuy <= 1) {
            limit_buy_level = value - trade_allow_level * (this->diffOffset_ + (trade_stock_value < value ? 1000 : 0));
            trade_limit_buy.SetValue(datetime + 1, limit_buy_level, pday_of_year);
            update_level_cooldown_seconds = this->update_level_interval_;
        } else {
            limit_buy_level = -numeric_limits<double>::max();
        }*/
        if (value > (currentPositionBuy - 1)) {
            double quote_step = GetMinimalQuoteStep(pday_of_year);
            limit_buy_level = trade_stock_value -
            round(trade_allow_level * this->diffOffset_ *
                  max(1.0 / (1.0 + (pow(fabs(currentPositionBuy - 1 - value), 2.0))), 0.1) / quote_step) * floor(quote_step);
            trade_limit_buy.SetValue(datetime + 1, limit_buy_level, pday_of_year);
            update_level_cooldown_seconds = this->update_level_interval_;
        } else {
            limit_buy_level = -numeric_limits<double>::max();
        }
        
        double currentPositionSell = (current_position - ((current_position <= 0) ?
                                                          (1.0 / trade_allow_level) :
                                                          (2.0 - 1.0 / trade_allow_level)));
        /*if (currentPositionSell >= -1) {
            limit_sell_level = value + trade_allow_level * (this->diffOffset_ + (trade_stock_value > value ? 1000 : 0));
            trade_limit_sell.SetValue(datetime + 1, limit_sell_level, pday_of_year);
            update_level_cooldown_seconds = this->update_level_interval_;
        } else {
            limit_sell_level = numeric_limits<double>::max();
        }*/
        if (value < (currentPositionSell + 1)) {
            double quote_step = GetMinimalQuoteStep(pday_of_year);
            limit_sell_level = trade_stock_value +
            round(trade_allow_level * this->diffOffset_ *
                  max(1.0 / (1.0 + (pow(fabs(value - currentPositionSell - 1), 2.0))), 0.1) / quote_step) * floor(quote_step);
            trade_limit_sell.SetValue(datetime + 1, limit_sell_level, pday_of_year);
            update_level_cooldown_seconds = this->update_level_interval_;
        } else {
            limit_sell_level = numeric_limits<double>::max();
        }
    } else {
        if (limit_buy_level > -numeric_limits<double>::max()) {
            trade_limit_buy.SetValue(datetime + 1, limit_buy_level, pday_of_year);
        }
        if (limit_sell_level < numeric_limits<double>::max()) {
            trade_limit_sell.SetValue(datetime + 1, limit_sell_level, pday_of_year);
        }
        update_level_cooldown_seconds--;
    }
    
    if (timeout_after_deal_seconds) {
        timeout_after_deal_seconds--;
    }

    return return_operation;
}

void Trader::makeDeal(operationType signal, double& current_account,
                      int& current_position, double& current_profit,
                      double limit_buy_level, double limit_sell_level, double si, std::shared_ptr<DayOfTheYear> pday_of_year) const {
    if (signal != operationType::STAY) {
        current_account += ((signal == operationType::BUY) ? (-1 * limit_buy_level) : limit_sell_level);
        current_position += ((signal == operationType::BUY) ? 1 : -1);
        if (current_position == 0) {
            current_profit = current_account - (4.0 / ((double)si / (double)5000.0)) * GetMinimalQuoteStep(pday_of_year);
        }
    }
}

double Trader::CalculateTradeFitness(const Series& trade_position, const Series& trade_account, const Series& trade_account_moment, const Series& trade_signal, const std::shared_ptr<Operator>& strategy) {
    struct tm time_struct;
    unsigned long deal_count = 0;
    unsigned long deal_count_profit = 0;
    double deal_ammount_profit = 0.0;
    double max_moment_loss_quadratic_ammount = 0.0;
    unsigned long deal_sum_time = 0;
    double signal_bound = 0.0;
    long signal_bound_count = 0;
    vector<double> day_statistics;
    
    // Мультипликатор реинвестиции
    double amount_multiplier = 1.0;
    double deal_ammount_profit_max = deal_ammount_profit;
    
    const ulong long_time_penalty_delta = 1200;
    const ulong short_time_penalty_delta = 60;
    const double amount_multiplier_correction = 2000.0;

    for (auto& datetime_series : trade_position.datetime_map_) {
        unsigned long offset = 0;
        unsigned long deal_start_offset = 0;
        bool deal_started = false;
        float max_loss_moment = 0.0;
        std::shared_ptr<DayOfTheYear> pday_of_year =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        
        for (auto& sample : datetime_series.second.series_) {
            signal_bound += pow(0.1 * fabs(trade_signal.GetValue(datetime_series.second.begin_interval + offset, pday_of_year)), 2.0);
            signal_bound_count++;
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
                
                deal_ammount_profit += (amount_diff > 0) ? (amount_multiplier * amount_diff /
                                        pow(max(long_time_penalty_delta, (offset - deal_start_offset)) / (double)long_time_penalty_delta, 0.5) *
                                            (min((double)short_time_penalty_delta, (double)(offset - deal_start_offset)) /
                                                (double)short_time_penalty_delta)) : 0.0;

                deal_ammount_profit -= (amount_diff < 0) ? (amount_multiplier * (-amount_diff)) : 0.0;

                max_moment_loss_quadratic_ammount += pow(0.001 * max_loss_moment, 2);
                deal_count++;
                if (amount_diff > GetMinimalQuoteStep(pday_of_year)) {
                    deal_count_profit++;
                }
                
                if (deal_ammount_profit > deal_ammount_profit_max) {
                    deal_ammount_profit_max = deal_ammount_profit;
                    amount_multiplier = 1.0 + deal_ammount_profit_max / amount_multiplier_correction;
                }
            }
            offset++;
        }
        //day_statistics.push_back(deal_ammount_profit - deal_ammount_losses - (!day_statistics.empty() ? day_statistics.back() : 0.0));
    }
    
    /*double day_average = 0.0;
    for (auto day_stat : day_statistics) {
        day_average += day_stat / (double)day_statistics.size();
    }
    double day_dispersion = 0.0;
    for (auto day_stat : day_statistics) {
        day_dispersion += pow(day_stat - day_average, 2.0);
    }*/

    return (deal_count > 0) ? /*(1.0 + sqrt(deal_count_profit)) */
    ((deal_ammount_profit) < 0 ? (-1.0 * log(1.0 - deal_ammount_profit)) :
        log(1.0 + deal_ammount_profit)) /
        sqrt(1.0 + (double)signal_bound / (double)signal_bound_count)
        /* (1.0 + sqrt(day_dispersion)) /* (1.0 + sqrt((double)signal_bound / (double)signal_bound_count)) /* (1.0 + log10(10.0 + 0 * max_moment_loss_quadratic_ammount) * (1.0 + 0.0 * sqrt(signal_bound / (double)signal_bound_count)) * log10(10.0 + 0 * strategy->ToString().length()))*/
        : 0;
}

void Trader::SetStock(std::shared_ptr<Series> stock, std::shared_ptr<Series> stock_min, std::shared_ptr<Series> stock_max,
                      std::shared_ptr<Series> stock_si) {
    this->tradeStock_ = stock;
    this->tradeStockMin_ = stock_min;
    this->tradeStockMax_ = stock_max;
    this->trade_si_ = stock_si;
}

void Trader::SetAllowSeries(std::shared_ptr<Series> allow_series) {
    this->tradeAllowSignal_ = allow_series;
}

double Trader::GetMinimalQuoteStep(std::shared_ptr<DayOfTheYear> pday_of_year) {
    if ((pday_of_year->year_corrected + 1900) < 2012 ||
        ((pday_of_year->year_corrected + 1900) == 2012 && pday_of_year->day_of_year < 260)) {
        return 5.0;
    }
    
    return 10.0;
}