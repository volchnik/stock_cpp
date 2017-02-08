/*
* File:   Trader.cpp
* Author: volchnik
*
* Created on February 23, 2015, 11:56 AM
*/

#include "Trader.h"
#include "Operator/OperatorAdd.h"
#include "Operator/OperatorMultiply.h"
#include "Operator/OperatorSeries.h"

Trader::Trader(std::shared_ptr<Series> stock, std::shared_ptr<Series> stock_min,
               std::shared_ptr<Series> stock_max, std::shared_ptr<Series> stock_si,
               std::shared_ptr<Series> allow,
               long timeoutAfterDeal, long update_level_interval, long maxPosition,
               double diffOffset, double stop_loss_percent, unsigned long start_trade_offset) :
    tradeStock_(stock), tradeStockMin_(stock_min), tradeStockMax_(stock_max),
    trade_si_(stock_si),
    tradeAllowSignal_(allow),
    timeoutAfterDeal_(timeoutAfterDeal),
    update_level_interval_(update_level_interval),
    maxPosition_(maxPosition),
    timeoutAfterDealCooldown_(0),
    diffOffset_(diffOffset),
    stop_loss_percent_(stop_loss_percent),
    start_trade_offset_(start_trade_offset) {
}

Trader::~Trader() {
}

std::tuple<double,
           Series,
           Series,
           Series,
           Series,
           Series,
           Series> Trader::Trade(std::shared_ptr<Operator> signal_strategy) const {
  OperatorAdd resultOperatorAdd(std::make_shared<OperatorSeries>(std::make_shared<Series>(tradeStock_->GenerateZeroBaseSeries())),
                                signal_strategy);
  std::shared_ptr<Operator> resultOperator = resultOperatorAdd.perform();
  return this->Trade(*dynamic_cast<OperatorSeries *>(resultOperator.get())->getSeries());
}

std::tuple<double, Series, Series, Series, Series, Series, Series> Trader::Trade(const Series &trade_signal) const {
  struct tm time_struct;
  operationType signal;

  Series trade_position = trade_signal * 0.0;
  Series trade_account = trade_signal * 0.0;
  Series trade_account_moment = trade_signal * 0.0;

  Series trade_limit_buy(*this->tradeStock_);
  trade_limit_buy.SetName("trade_limit_buy");
  Series trade_limit_sell(*this->tradeStock_);
  trade_limit_sell.SetName("trade_limit_sell");

  int current_position = 0;
  double current_account = 0.0;
  double current_profit = 0.0;

  double limit_buy_level_fix_deal = -numeric_limits<double>::max();
  double limit_sell_level_fix_deal = numeric_limits<double>::max();

  long update_level_cooldown_seconds = 0;
  long timeout_after_deal_seconds = 0;
  unsigned long begin_trade_signal = trade_signal.datetime_map_.begin()->second.begin_interval;

  for (auto &datetime_series : trade_signal.datetime_map_) {
    unsigned long offset = 0;
    std::shared_ptr<DayOfTheYear> pday_of_year =
        std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));

    for (auto &sample : datetime_series.second.series_) {

      if (datetime_series.second.begin_interval + offset - begin_trade_signal < this->start_trade_offset_) {
        continue;
      }
      float trade_stock_value = this->tradeStock_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
      float trade_stock_min_value = this->tradeStockMin_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
      float trade_stock_max_value = this->tradeStockMax_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
      float trade_si = this->trade_si_->GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
      /*if (current_position != 0 && (current_account + current_position * trade_stock_value - current_profit) < -1 * maxPosition_ * trade_stock_value * stop_loss_percent_) {
          makeDeal(current_position > 0 ? operationType::SELL : operationType::BUY, current_account, current_position, current_profit, trade_stock_value, trade_stock_value, trade_si, pday_of_year);
      } else*/

      pair<double, double> limitDealLevels = GetLimitDealLevels(pday_of_year,
                                                                datetime_series.second.begin_interval + offset,
                                                                current_position,
                                                                sample.value,
                                                                trade_stock_value,
                                                                this->diffOffset_,
                                                                this->update_level_interval_,
                                                                update_level_cooldown_seconds,
                                                                this->tradeAllowSignal_,
                                                                trade_limit_buy,
                                                                trade_limit_sell,
                                                                false);
      if ((signal = GetCurrentSignal(trade_stock_min_value,
                                     trade_stock_max_value,
                                     current_position,
                                     limitDealLevels.first,
                                     limitDealLevels.second,
                                     limit_buy_level_fix_deal,
                                     limit_sell_level_fix_deal,
                                     timeout_after_deal_seconds,
                                     this->timeoutAfterDeal_,
                                     update_level_cooldown_seconds,
                                     maxPosition_))
          != operationType::STAY) {
        makeDeal(signal, current_account, current_position, current_profit,
                 limit_buy_level_fix_deal, limit_sell_level_fix_deal, trade_si, pday_of_year);
      }
      trade_position.SetValue(datetime_series.second.begin_interval + offset, current_position, pday_of_year);
      trade_account.SetValue(datetime_series.second.begin_interval + offset, current_profit, pday_of_year);

      trade_account_moment.SetValue(datetime_series.second.begin_interval + offset,
                                    (current_position != 0) ? (current_account + current_position * trade_stock_value
                                        - current_profit) : 0,
                                    pday_of_year);

      offset++;
    }
  }

  return std::make_tuple(current_account,
                         trade_position,
                         trade_account,
                         trade_limit_buy,
                         trade_limit_sell,
                         trade_account_moment,
                         trade_signal);
}

pair<double, double> Trader::GetLimitDealLevels(const std::shared_ptr<DayOfTheYear> &pday_of_year,
                                                const long &datetime,
                                                const int &current_position,
                                                const float &value,
                                                const float &trade_stock_value,
                                                const double &diffOffset,
                                                const long &update_level_interval,
                                                long &update_level_cooldown_seconds,
                                                const std::shared_ptr<Series> &tradeAllowSignal,
                                                Series &trade_limit_buy,
                                                Series &trade_limit_sell,
                                                const bool &is_limit_trade) {
  pair<double, double> limitLevels;
  limitLevels.first = -numeric_limits<double>::max();
  limitLevels.second = numeric_limits<double>::max();

  if (update_level_cooldown_seconds == 0) {
    float trade_allow_level = tradeAllowSignal->GetValue(datetime, pday_of_year);

    float currentPositionBuy = (current_position + ((current_position >= 0) ?
                                                    (1.0 / trade_allow_level) :
                                                    (2.0 - 1.0 / trade_allow_level)));
    if (is_limit_trade) {
      // Лимитная сделка
      if (value > (currentPositionBuy - 1)) {
        double quote_step = GetMinimalQuoteStep(pday_of_year);
        limitLevels.first = trade_stock_value -
            round(trade_allow_level * diffOffset *
                max(1.0 / (1.0 + (pow(fabs(currentPositionBuy - 1 - value), 2.0))), 0.1) / quote_step) * floor(quote_step);
        trade_limit_buy.SetValue(datetime + 1, limitLevels.first, pday_of_year);
        update_level_cooldown_seconds = update_level_interval;
      } else {
        limitLevels.first = -numeric_limits<double>::max();
      }
    } else {
      // Моментальная сделка
      if (value > (currentPositionBuy)) {
        limitLevels.first = numeric_limits<double>::max();
        trade_limit_buy.SetValue(datetime + 1, limitLevels.first, pday_of_year);
        update_level_cooldown_seconds = update_level_interval;
      } else {
        limitLevels.first = -numeric_limits<double>::max();
      }
    }

    double currentPositionSell = (current_position - ((current_position <= 0) ?
                                                      (1.0 / trade_allow_level) :
                                                      (2.0 - 1.0 / trade_allow_level)));
    if (is_limit_trade) {
      // Лимитная сделка
      if (value < (currentPositionSell + 1)) {
        double quote_step = GetMinimalQuoteStep(pday_of_year);
        limitLevels.second = trade_stock_value +
            round(trade_allow_level * diffOffset *
                max(1.0 / (1.0 + (pow(fabs(value - currentPositionSell - 1), 2.0))), 0.1) / quote_step) * floor(quote_step);
        trade_limit_sell.SetValue(datetime + 1, limitLevels.second, pday_of_year);
        update_level_cooldown_seconds = update_level_interval;
      } else {
        limitLevels.second = numeric_limits<double>::max();
      }
    } else {
      // Моментальная сделка
      if (value < (currentPositionSell)) {
        limitLevels.second = -numeric_limits<double>::max();
        trade_limit_sell.SetValue(datetime + 1, limitLevels.second, pday_of_year);
        update_level_cooldown_seconds = update_level_interval;
      } else {
        limitLevels.second = numeric_limits<double>::max();
      }
    }
  } else {
    if (limitLevels.first > -numeric_limits<double>::max()) {
      trade_limit_buy.SetValue(datetime + 1, limitLevels.first, pday_of_year);
    }
    if (limitLevels.second < numeric_limits<double>::max()) {
      trade_limit_sell.SetValue(datetime + 1, limitLevels.second, pday_of_year);
    }
    update_level_cooldown_seconds--;
  }

  return limitLevels;
}

Trader::operationType Trader::GetCurrentSignal(const float &trade_stock_min_value,
                                                      const float &trade_stock_max_value,
                                                      const int &current_position,
                                                      const double &limit_buy_level,
                                                      const double &limit_sell_level,
                                                      double &limit_buy_level_fix_deal,
                                                      double &limit_sell_level_fix_deal,
                                                      long &timeout_after_deal_seconds,
                                                      const long &timeoutAfterDeal,
                                                      long &update_level_cooldown_seconds,
                                                      const long &max_position)
{

  Trader::operationType return_operation = operationType::STAY;

  if ((current_position < 0 || max_position > labs(current_position)) &&
      timeout_after_deal_seconds == 0 &&
      trade_stock_min_value <= limit_buy_level) {

    update_level_cooldown_seconds = 0;
    timeout_after_deal_seconds = timeoutAfterDeal;
    return_operation = operationType::BUY;
    limit_buy_level_fix_deal = (trade_stock_max_value + trade_stock_min_value) * 0.5;

  } else if ((current_position > 0 || max_position > labs(current_position)) &&
      timeout_after_deal_seconds == 0 &&
      trade_stock_max_value >= limit_sell_level) {

    update_level_cooldown_seconds = 0;
    timeout_after_deal_seconds = timeoutAfterDeal;
    return_operation = operationType::SELL;
    limit_sell_level_fix_deal = (trade_stock_max_value + trade_stock_min_value) * 0.5;
  }

  if (timeout_after_deal_seconds) {
    timeout_after_deal_seconds--;
  }

  return return_operation;
}

void Trader::makeDeal(operationType signal,
                      double &current_account,
                      int &current_position,
                      double &current_profit,
                      double limit_buy_level,
                      double limit_sell_level,
                      double si,
                      std::shared_ptr<DayOfTheYear> pday_of_year) const {
  if (signal != operationType::STAY) {
    current_account += ((signal == operationType::BUY) ? (-1 * limit_buy_level) : limit_sell_level) -
        (2.0 / ((double) si / (double) 5000.0)) * GetMinimalQuoteStep(pday_of_year);
    current_position += ((signal == operationType::BUY) ? 1 : -1);
    if (current_position == 0) {
      current_profit = current_account;
    }
  }
}

double Trader::CalculateTradeFitness(const Series &trade_position,
                                     const Series &trade_account,
                                     const std::shared_ptr<Operator> &strategy) {
  struct tm time_struct;
  double deal_amount_profit = 0.0;
  vector<double> day_statistics;

  long deal_count = 0;
  long positive_deal_count = 0;

  for (auto &datetime_series : trade_position.datetime_map_) {
    ulong offset = 0;
    ulong deal_start_offset = 0;

    bool deal_started = false;
    std::shared_ptr<DayOfTheYear> pday_of_year =
        std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));

    for (auto &sample : datetime_series.second.series_) {
      float trade_position_current = trade_position.GetValue(datetime_series.second.begin_interval + offset, pday_of_year);
      if (!deal_started && trade_position_current != 0) {
        deal_started = true;
        deal_start_offset = offset;
      } else if (deal_started && trade_position_current == 0) {
        deal_started = false;
        double amount_diff = trade_account.GetValue(datetime_series.second.begin_interval + offset, pday_of_year) -
            trade_account.GetValue(datetime_series.second.begin_interval + offset - 1, pday_of_year);

        if (amount_diff > 0) {
          amount_diff /= max((ulong)MAX_DEAL_TIME_PENALTY, (offset - deal_start_offset)) / (double)MAX_DEAL_TIME_PENALTY;
          amount_diff *= min((ulong)MIN_DEAL_TIME_PENALTY, (offset - deal_start_offset)) / (double)MIN_DEAL_TIME_PENALTY;
        }

        deal_amount_profit += amount_diff;

        deal_count++;
        if (amount_diff > 0.0) {
          positive_deal_count++;
        }
      }
      offset++;
    }
  }

  return deal_amount_profit * positive_deal_count / (deal_count > 0 ? (double)deal_count : 1);
}

void Trader::SetStock(std::shared_ptr<Series> stock, std::shared_ptr<Series> stock_min, std::shared_ptr<Series> stock_max,
                      std::shared_ptr<Series> stock_si) {
  this->tradeStock_ = stock;
  this->tradeStockMin_ = stock_min;
  this->tradeStockMax_ = stock_max;
  this->trade_si_ = stock_si;
}

void Trader::SetAllowSeries(std::shared_ptr<Series> allow_series) {
  this->tradeAllowSignal_ = allow_series;
}

double Trader::GetMinimalQuoteStep(std::shared_ptr<DayOfTheYear> pday_of_year) {
  if ((pday_of_year->year_corrected + 1900) < 2012 ||
      ((pday_of_year->year_corrected + 1900) == 2012 && pday_of_year->day_of_year < 260)) {
    return 5.0;
  }

  return 10.0;
}