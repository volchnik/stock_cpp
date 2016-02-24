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
    Trader(std::shared_ptr<Series> stock, std::shared_ptr<Series> allow,
        long timeoutAfterDeal, long update_level_interval, long maxPosition, double diffOffsetPersent, double stop_loss_percent);
    virtual ~Trader();
    
    std::tuple<double, Series, Series, Series, Series, Series> Trade(const Series& trade_signal) const;
    std::tuple<double, Series, Series, Series, Series, Series> Trade(std::shared_ptr<Operator> signal_strategy) const;
    
    enum operationType {STAY, BUY, SELL};
    
    static double CalculateTradeFitness(const Series& trade_position, const Series& trade_account, const Series& trade_account_moment);
private:
    long timeoutAfterDeal_;
    long timeoutAfterDealCooldown_;
    long maxPosition_;
    double stop_loss_percent_;

    std::shared_ptr<Series> tradeStock_;
    std::shared_ptr<Series> tradeAllowSignal_;
    // Величина спреда для совершения сделок по сигналу в процентах от базового актива
    double diffOffsetPersent_;
    
    long update_level_interval_;
    
    operationType GetCurrentSignal(long datetime, std::shared_ptr<DayOfTheYear> pday_of_year, float value, float trade_stock_value, int current_position, Series& trade_limit_buy, Series& trade_limit_sell, double& limit_buy_level, double& limit_sell_level, long& update_level_cooldown_seconds, long& timeout_after_deal_seconds) const;
    void makeDeal(operationType signal, float trade_stock_value, double& current_account, int& current_position, double& current_profit) const;
};

#endif	/* TRADER_H */

