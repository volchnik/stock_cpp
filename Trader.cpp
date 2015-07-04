/* 
 * File:   Trader.cpp
 * Author: volchnik
 * 
 * Created on February 23, 2015, 11:56 AM
 */

#include "Trader.h"

Trader::Trader(const Series& stock, const Series& signal, const Series& allow, 
        long timeoutAfterDeal, long updateLevelInterval, long maxPosition, double diffOffsetPersent) :
    tradeStock_(stock), tradeSignal_(signal), tradeAllowSignal_(allow), 
    timeoutAfterDeal_(timeoutAfterDeal), currenPosition_(0), currentAccount_(0.0),
    updateLevelCooldownSeconds_(0), updateLevelInterval_(updateLevelInterval),
    maxPosition_(maxPosition), timeoutAfterDealCooldown_(0), diffOffsetPersent_(diffOffsetPersent),
    tradeLimitBuy_(stock), tradeLimitSell_(stock) {

    this->tradePosition_ = signal * 0.0;
    this->tradeAccount_ = signal * 0.0;
}

Trader::~Trader() {
}

double Trader::Trade() {
    operationType signal;

    for (auto& sample : this->tradeStock_.series_) {
        if ((signal = GetCurrentSignal(sample.datetime)) != operationType::STAY) {
            makeDeal(signal, sample.datetime);
        }
        this->tradePosition_.SetValue(sample.datetime, this->currenPosition_);
        this->tradeAccount_.SetValue(sample.datetime, this->currentProfit_);
    }
    return this->currentAccount_;
}

Trader::operationType Trader::GetCurrentSignal(long datetime) {
    
    if (this->updateLevelCooldownSeconds_ == 0) {
        this->updateLevelCooldownSeconds_ = this->updateLevelInterval_;
        // Обновляем лимитные уровни
        double currenPositionBuy = currenPosition_ + ((currenPosition_ >= 0) ?
            (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
            (2.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)));
        if (this->tradeSignal_.GetValue(datetime) >= (currenPositionBuy - 1.0)) {
            this->limitBuyLevel_ = this->tradeStock_.GetValue(datetime) -
                    this->diffOffsetPersent_ * this->tradeStock_.GetValue(datetime) *
                    max(currenPositionBuy - this->tradeSignal_.GetValue(datetime), 0.0);
            this->tradeLimitBuy_.SetValue(datetime, this->limitBuyLevel_);
        } else {
            this->limitBuyLevel_ = -numeric_limits<double>::max();
        }
        
        double currenPositionSell = currenPosition_ - ((currenPosition_ <= 0) ?
            (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
            (2.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)));
        if (this->tradeSignal_.GetValue(datetime) <= (currenPositionSell + 1.0)) {
            this->limitSellLevel_ = this->tradeStock_.GetValue(datetime) +
                    this->diffOffsetPersent_ * this->tradeStock_.GetValue(datetime) *
                    max(this->tradeSignal_.GetValue(datetime) - currenPositionSell, 0.0);
            this->tradeLimitSell_.SetValue(datetime, this->limitSellLevel_);
        } else {
            this->limitSellLevel_ = numeric_limits<double>::max();
        }
    } else {
        if (this->limitBuyLevel_ > -numeric_limits<double>::max()) {
            this->tradeLimitBuy_.SetValue(datetime, this->limitBuyLevel_);
        }
        if (this->limitSellLevel_ < numeric_limits<double>::max()) {
            this->tradeLimitSell_.SetValue(datetime, this->limitSellLevel_);
        }
        this->updateLevelCooldownSeconds_--;
    }
    
    if (this->timeoutAfterDealCooldown_) {
        this->timeoutAfterDealCooldown_--;
    }
    
    if ((this->currenPosition_ < 0 || maxPosition_ > labs(this->currenPosition_)) &&
            this->timeoutAfterDealCooldown_ == 0 &&
            this->tradeStock_.GetValue(datetime) <= this->limitBuyLevel_) {
        this->updateLevelCooldownSeconds_ = 0;
        this->timeoutAfterDealCooldown_ = this->timeoutAfterDeal_;
        return operationType::BUY;
    } else if ((this->currenPosition_ > 0 ||maxPosition_ > labs(this->currenPosition_)) &&
            this->timeoutAfterDealCooldown_ == 0 &&
            this->tradeStock_.GetValue(datetime) >= this->limitSellLevel_) {
        this->updateLevelCooldownSeconds_ = 0;
        this->timeoutAfterDealCooldown_ = this->timeoutAfterDeal_;
        return operationType::SELL;
    }

    return operationType::STAY;
}

void Trader::makeDeal(operationType Signal, long datetime){
    if (Signal != operationType::STAY) {
        this->currentAccount_ += ((Signal == operationType::BUY) ? -1 : 1) * this->tradeStock_.GetValue(datetime);
        this->currenPosition_ += ((Signal == operationType::BUY) ? 1 : -1);
        if (this->currenPosition_ == 0) {
            this->currentProfit_ = this->currentAccount_;
        }
    }
}