/* 
 * File:   Trader.cpp
 * Author: volchnik
 * 
 * Created on February 23, 2015, 11:56 AM
 */

#include "Trader.h"
#include "Operator/OperatorAdd.h"
#include "Operator/OperatorSeries.h"

Trader::Trader(const Series& stock, const Series& allow,
        long timeoutAfterDeal, long update_level_interval, long maxPosition, double diffOffsetPersent) :
    tradeStock_(stock), tradeAllowSignal_(allow),
    timeoutAfterDeal_(timeoutAfterDeal), update_level_interval_(update_level_interval),
    maxPosition_(maxPosition), timeoutAfterDealCooldown_(0), diffOffsetPersent_(diffOffsetPersent) {
}

Trader::~Trader() {
}

std::tuple<double, Series, Series, Series, Series> Trader::Trade(std::shared_ptr<Operator> signal_strategy) {
    OperatorAdd resultOperatorAdd(std::make_shared<OperatorSeries>(std::make_shared<Series>(tradeStock_.GenerateZeroBaseSeries())),
                                  signal_strategy);
    std::shared_ptr<Operator> resultOperator = resultOperatorAdd.perform();
    return this->Trade(*dynamic_cast<OperatorSeries*>(resultOperator.get())->getSeries());
}

std::tuple<double, Series, Series, Series, Series> Trader::Trade(const Series& trade_signal) {
    operationType signal;
    
    Series trade_position = trade_signal * 0.0;
    Series trade_account = trade_signal * 0.0;
    
    Series trade_limit_buy(trade_signal);
    Series trade_limit_sell(trade_signal);
    
    int curren_position = 0;
    double current_account = 0.0;
    double current_profit = 0.0;
    
    double limit_buy_level = 0.0;
    double limit_sell_level = 0.0;
    long update_level_cooldown_seconds = 0;

    for (auto& sample : trade_signal.series_) {
        if ((signal = GetCurrentSignal(sample.datetime, sample.value, curren_position, trade_limit_buy, trade_limit_sell,
                                       limit_buy_level, limit_sell_level, update_level_cooldown_seconds))
            != operationType::STAY) {
            makeDeal(signal, sample.datetime, current_account, curren_position, current_profit);
        }
        trade_position.SetValue(sample.datetime, curren_position);
        trade_account.SetValue(sample.datetime, current_profit);
    }
    return std::make_tuple(current_account, trade_position, trade_account, trade_limit_buy, trade_limit_sell);
}

Trader::operationType Trader::GetCurrentSignal(long datetime, double value, int current_position,
                                               Series& trade_limit_buy, Series& trade_limi_sell,
                                               double& limit_buy_level, double& limit_sell_level, long& update_level_cooldown_seconds) {
    if (update_level_cooldown_seconds == 0) {
        update_level_cooldown_seconds = this->update_level_interval_;
        // Обновляем лимитные уровни
        double currenPositionBuy = current_position + ((current_position >= 0) ?
            (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
            (2.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)));
        if (value >= (currenPositionBuy - 1.0)) {
            limit_buy_level = this->tradeStock_.GetValue(datetime) -
                    this->diffOffsetPersent_ * this->tradeStock_.GetValue(datetime) *
                    max(currenPositionBuy - value, 0.0);
            trade_limit_buy.SetValue(datetime, limit_buy_level);
        } else {
            limit_buy_level = -numeric_limits<double>::max();
        }
        
        double currenPositionSell = current_position - ((current_position <= 0) ?
            (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
            (2.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)));
        if (value <= (currenPositionSell + 1.0)) {
            limit_sell_level = this->tradeStock_.GetValue(datetime) +
                    this->diffOffsetPersent_ * this->tradeStock_.GetValue(datetime) *
                    max(value - currenPositionSell, 0.0);
            trade_limi_sell.SetValue(datetime, limit_sell_level);
        } else {
            limit_sell_level = numeric_limits<double>::max();
        }
    } else {
        if (limit_buy_level > -numeric_limits<double>::max()) {
            trade_limit_buy.SetValue(datetime, limit_buy_level);
        }
        if (limit_sell_level < numeric_limits<double>::max()) {
            trade_limi_sell.SetValue(datetime, limit_sell_level);
        }
        update_level_cooldown_seconds--;
    }
    
    if (this->timeoutAfterDealCooldown_) {
        this->timeoutAfterDealCooldown_--;
    }
    
    if ((current_position < 0 || maxPosition_ > labs(current_position)) &&
            this->timeoutAfterDealCooldown_ == 0 &&
            this->tradeStock_.GetValue(datetime) <= limit_buy_level) {
        update_level_cooldown_seconds = 0;
        this->timeoutAfterDealCooldown_ = this->timeoutAfterDeal_;
        return operationType::BUY;
    } else if ((current_position > 0 ||maxPosition_ > labs(current_position)) &&
            this->timeoutAfterDealCooldown_ == 0 &&
            this->tradeStock_.GetValue(datetime) >= limit_sell_level) {
        update_level_cooldown_seconds = 0;
        this->timeoutAfterDealCooldown_ = this->timeoutAfterDeal_;
        return operationType::SELL;
    }

    return operationType::STAY;
}

void Trader::makeDeal(operationType signal, long datetime, double& current_account, int& current_position, double& current_profit){
    if (signal != operationType::STAY) {
        current_account += ((signal == operationType::BUY) ? -1 : 1) * this->tradeStock_.GetValue(datetime);
        current_position += ((signal == operationType::BUY) ? 1 : -1);
        if (current_position == 0) {
            current_profit = current_account;
        }
    }
}