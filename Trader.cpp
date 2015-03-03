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
    TraderOperation tradeOperation;
    TraderOperation::operationType signal;

    for (auto& sample : this->tradeStock_.series_) {
        if ((signal = GetCurrentSignal(sample.datetime)) != TraderOperation::operationType::STAY) {
            if (tradeOperation.GetDuration() == 0) {
                // Случайное проскальзывание
                tradeOperation.SetDuration(rand() % 3);
                tradeOperation.SetOperationType(signal);
            }
        }
        if (tradeOperation.GetOperationType() != TraderOperation::operationType::STAY &&
                tradeOperation.GetDuration() == 0) {
            makeDeal(tradeOperation.GetOperationType(), sample.datetime);
            tradeOperation.SetOperationType(TraderOperation::operationType::STAY);
            // Таймаут после совершения сделки
            tradeOperation.SetDuration(2);
        } else if (tradeOperation.GetDuration() != 0) {
            tradeOperation.SetDuration(tradeOperation.GetDuration() - 1);
        }
    }
    return this->currentAccount;
}

TraderOperation::operationType Trader::GetCurrentSignal(long datetime) {
    if (this->tradeSignal_.GetValue(datetime) > currenPosition + 
            ((currenPosition >= 0.0) ? (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
                (1.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)))) {
        return TraderOperation::operationType::BUY;
    } else if (this->tradeSignal_.GetValue(datetime) < currenPosition - 
            ((currenPosition <= 0.0) ? (1.0 / this->tradeAllowSignal_.GetValue(datetime)) :
                (1.0 - 1.0 / this->tradeAllowSignal_.GetValue(datetime)))) {
        return TraderOperation::operationType::SELL;
    }
    return TraderOperation::operationType::STAY;
}

void Trader::makeDeal(TraderOperation::operationType Signal, long datetime){
    if (Signal != TraderOperation::operationType::STAY) {
        this->currentAccount += ((Signal == TraderOperation::operationType::BUY) ? -1 : 1) * this->tradeStock_.GetValue(datetime);
        this->currenPosition += ((Signal == TraderOperation::operationType::BUY) ? 1 : -1);
    }
}