/* 
 * File:   Trader.h
 * Author: volchnik
 *
 * Created on February 23, 2015, 11:56 AM
 */
#include "root.h"
#include "Series.h"
#include "Operator/Operator.h"

#ifndef TRADER_H
#define TRADER_H

class Trader {
 public:
  Trader(std::shared_ptr<Series> stock,
         std::shared_ptr<Series> stock_min,
         std::shared_ptr<Series> stock_max,
         std::shared_ptr<Series> stock_si,
         std::shared_ptr<Series> allow,
         long timeoutAfterDeal,
         long update_level_interval,
         long maxPosition,
         double diffOffset,
         double stop_loss_percent,
         unsigned long start_trade_offset);
  virtual ~Trader();

  std::tuple<double, Series, Series, Series, Series, Series, Series> Trade(const Series &trade_signal) const;
  std::tuple<double, Series, Series, Series, Series, Series, Series> Trade(std::shared_ptr<Operator> signal_strategy) const;

  enum operationType { STAY, BUY, SELL };

  static double CalculateTradeFitness(const Series &trade_position,
                                      const Series &trade_account,
                                      const Series &trade_account_moment,
                                      const Series &trade_signal,
                                      const std::shared_ptr<Operator> &strategy);
  void SetStock(std::shared_ptr<Series> stock,
                std::shared_ptr<Series> stock_min,
                std::shared_ptr<Series> stock_max,
                std::shared_ptr<Series> stock_si);
  void SetAllowSeries(std::shared_ptr<Series> allow_series);

  static double GetMinimalQuoteStep(std::shared_ptr<DayOfTheYear> pday_of_year);

 private:
  long timeoutAfterDeal_;
  long timeoutAfterDealCooldown_;
  long maxPosition_;
  double stop_loss_percent_;
  unsigned long start_trade_offset_;

  std::shared_ptr<Series> tradeStock_ = nullptr;
  std::shared_ptr<Series> tradeStockMin_ = nullptr;
  std::shared_ptr<Series> tradeStockMax_ = nullptr;
  std::shared_ptr<Series> tradeAllowSignal_ = nullptr;
  std::shared_ptr<Series> trade_si_ = nullptr;

  // Величина спреда для совершения сделок по сигналу (абсолютный дифференс)
  double diffOffset_;

  long update_level_interval_;

  pair<double, double> GetLimitDealLevels(const std::shared_ptr<DayOfTheYear> &pday_of_year,
                                                  const long &datetime,
                                                  const int &current_position,
                                                  const float &value,
                                                  const float &trade_stock_value,
                                                  const double &diffOffset,
                                                  const long &update_level_interval,
                                                  long &update_level_cooldown_seconds,
                                                  const std::shared_ptr<Series> &tradeAllowSignal,
                                                  Series &trade_limit_buy,
                                                  Series &trade_limit_sell) const;

  operationType GetCurrentSignal(const float &trade_stock_min_value,
                                 const float &trade_stock_max_value,
                                 const int &current_position,
                                 const double &limit_buy_level,
                                 const double &limit_sell_level,
                                 double &limit_buy_level_fix_deal,
                                 double &limit_sell_level_fix_deal,
                                 long &timeout_after_deal_seconds,
                                 const long &timeoutAfterDeal,
                                 long &update_level_cooldown_seconds) const;

  void makeDeal(operationType signal,
                double &current_account,
                int &current_position,
                double &current_profit,
                double limit_buy_level,
                double limit_sell_level,
                double si,
                std::shared_ptr<DayOfTheYear> pday_of_year) const;
};

#endif	/* TRADER_H */

