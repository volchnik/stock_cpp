/* 
 * File:   Trader.h
 * Author: volchnik
 *
 * Created on February 23, 2015, 11:56 AM
 */
#include "Series.h"

#ifndef TRADER_H
#define	TRADER_H

class Trader {
public:
    Trader(const Series& stock, const Series& signal, const Series& allow);
    virtual ~Trader();
    
    double Trade();
private:
    enum signal {STAY, BUY, SELL};
    Series tradeStock_;
    Series tradeSignal_;
    Series tradeAllowSignal_;
    
    int currenPosition;
    double currentAccount;
    
    signal GetCurrentSignal(long datetime);
    void makeDeal(signal Signal, long datetime);
};

#endif	/* TRADER_H */

