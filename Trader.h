/* 
 * File:   Trader.h
 * Author: volchnik
 *
 * Created on February 23, 2015, 11:56 AM
 */
#include "Series.h"
#include "TraderOperation.h"

#ifndef TRADER_H
#define	TRADER_H

class Trader {
public:
    Trader(const Series& stock, const Series& signal, const Series& allow);
    virtual ~Trader();
    
    double Trade();
    
    Series GetTradePosition() { return tradePosition_; }
    Series GetTradeAccount() { return tradeAccount_; }
private:
    Series tradeStock_;
    Series tradeSignal_;
    Series tradeAllowSignal_;
    
    Series tradePosition_;
    Series tradeAccount_;
    
    int currenPosition;
    double currentAccount;
    double currentProfit;
    
    TraderOperation::operationType GetCurrentSignal(long datetime);
    void makeDeal(TraderOperation::operationType Signal, long datetime);
};

#endif	/* TRADER_H */

