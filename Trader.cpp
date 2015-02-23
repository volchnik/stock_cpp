/* 
 * File:   Trader.cpp
 * Author: volchnik
 * 
 * Created on February 23, 2015, 11:56 AM
 */

#include "Trader.h"

Trader::Trader(const Series& stock, const Series& signal, const Series& allow) {
    this->tradeStock_ = stock;
    this->tradeSignal_ = signal;
    this->tradeAllowSignal_ = allow;
    
    currenPosition = 0;
    currentAccount = 0.0;
}

Trader::~Trader() {
}

double Trader::Trade() {
    for (auto& sample : this->tradeStock_.series_) {
        makeDeal(GetCurrentSignal(sample.datetime), sample.datetime);
    }
    return this->currentAccount;
}

Trader::signal Trader::GetCurrentSignal(long datetime) {
    if (this->tradeSignal_.GetValue(datetime) > currenPosition + 
            ((currenPosition >= 0.0) ? (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
                (1.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)))) {
        return signal::BUY;
    } else if (this->tradeSignal_.GetValue(datetime) < currenPosition - 
            ((currenPosition <= 0.0) ? (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
                (1.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)))) {
        return signal::SELL;
    }
    return signal::STAY;
}

void Trader::makeDeal(signal Signal, long datetime){
    if (Signal != signal::STAY) {
        this->currentAccount += ((Signal == signal::BUY) ? -1 : 1) * this->tradeStock_.GetValue(datetime);
        this->currenPosition += ((Signal == signal::BUY) ? 1 : -1);
    }
}