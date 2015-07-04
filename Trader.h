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
    Trader(const Series& stock, const Series& signal, const Series& allow, 
        long timeoutAfterDeal, long updateLevelInterval, long maxPosition, double diffOffsetPersent);
    virtual ~Trader();
    
    double Trade();
    
    Series GetTradePosition() { return tradePosition_; }
    Series GetTradeAccount() { return tradeAccount_; }
    Series GetTradeLimitBuy() { return tradeLimitBuy_; }
    Series GetTradeLimitSell() { return tradeLimitSell_; }
    
    enum operationType {STAY, BUY, SELL};
private:
    long timeoutAfterDeal_;
    long timeoutAfterDealCooldown_;
    long maxPosition_;

    Series tradeStock_;
    Series tradeSignal_;
    Series tradeAllowSignal_;
    // Величина спреда для совершения сделок по сигналу в процентах от базового актива
    double diffOffsetPersent_;
    
    // Лимитные заявки
    double limitBuyLevel_;
    double limitSellLevel_;
    long updateLevelCooldownSeconds_;
    long updateLevelInterval_;
    
    // Текущее состояние торговли
    int currenPosition_;
    double currentAccount_;
    double currentProfit_;
    
    Series tradePosition_;
    Series tradeAccount_;
    Series tradeLimitBuy_;
    Series tradeLimitSell_;
    
    operationType GetCurrentSignal(long datetime);
    void makeDeal(operationType Signal, long datetime);
};

#endif	/* TRADER_H */

