/* 
 * File:   Trader.h
 * Author: volchnik
 *
 * Created on February 23, 2015, 11:56 AM
 */
#include "Series.h"
#include "Operator/Operator.h"

#ifndef TRADER_H
#define	TRADER_H

class Trader {
public:
    Trader(const Series& stock, const Series& allow,
        long timeoutAfterDeal, long update_level_interval, long maxPosition, double diffOffsetPersent);
    virtual ~Trader();
    
    std::tuple<double, Series, Series, Series, Series> Trade(const Series& trade_signal);
    std::tuple<double, Series, Series, Series, Series> Trade(std::shared_ptr<Operator> signal_strategy);
    
    enum operationType {STAY, BUY, SELL};
private:
    long timeoutAfterDeal_;
    long timeoutAfterDealCooldown_;
    long maxPosition_;

    Series tradeStock_;
    Series tradeAllowSignal_;
    // Величина спреда для совершения сделок по сигналу в процентах от базового актива
    double diffOffsetPersent_;
    
    long update_level_interval_;
    
    operationType GetCurrentSignal(long datetime, double value, int current_position, Series& trade_limit_buy, Series& trade_limi_sell,
                                   double& limit_buy_level, double& limit_sell_level, long& update_level_cooldown_seconds);
    void makeDeal(operationType signal, long datetime, double& current_account, int& current_position, double& current_profit);
};

#endif	/* TRADER_H */

