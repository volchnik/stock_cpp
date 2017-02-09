/* 
 * File:   Generation.cpp
 * Author: volchnik
 * 
 * Created on June 7, 2015, 5:16 PM
 */

#include "../ThreadPool.h"
#include "Generation.h"
#include "../Series.h"
#include "../Operator/OperatorNumber.h"
#include "../Operator/OperatorSeries.h"
#include "../Operator/OperatorSma.h"
#include "../Operator/OperatorEma.h"
#include "../Operator/OperatorAtan.h"
#include "../Operator/OperatorMultiply.h"
#include "../Operator/OperatorAdd.h"
#include "../Operator/OperatorSubtract.h"
#include "../Operator/OperatorDivide.h"

Generation::Generation(const unsigned long &generation_count, const map<std::string, std::shared_ptr<Series>> &series_collection,
                       Trader trader, bool debug_output)
    : generation_count_(generation_count), series_collection_(series_collection), trader_(trader), debug_output_(debug_output) {
}

Generation::Generation(const Generation &orig)
    : strategy_(orig.strategy_), generation_count_(orig.generation_count_), trader_(orig.trader_),
      debug_output_(orig.debug_output_) {
}

Generation::~Generation() {
}

std::shared_ptr<Operator> Generation::GenerateRandom(unsigned long max_genarate_depth) {
  std::shared_ptr<Operator> return_operator;

  if (max_genarate_depth > 1) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution{operator_type_binary_distribution_,
                                                 operator_type_unary_distribution_, operator_type_finite_distribution_};
    unsigned long operator_variant = distribution(gen);
    switch (operator_variant) {
      case 0: // operator_type_binary_distribution_
      {
        std::discrete_distribution<int> distribution_binary{operator_binary_multiply_distribution_,
                                                            operator_binary_add_distribution_,
                                                            operator_binary_subtract_distribution_,
                                                            operator_binary_divide_distribution_};
        unsigned long operator_variant_binary = distribution_binary(gen);
        switch (operator_variant_binary) {
          case 0: // operator_binary_multiply_distribution_
          {
            OperatorMultiply operator_multiply(GenerateFiniteRandom(1),
                                               GenerateRandom(max_genarate_depth - 1));
            return std::make_shared<OperatorMultiply>(operator_multiply);
          }
            break;
          case 1: // operator_binary_add_distribution_
          {
            OperatorAdd operator_add(GenerateRandom(max_genarate_depth - 1),
                                     GenerateRandom(max_genarate_depth - 1));
            return std::make_shared<OperatorAdd>(operator_add);
          }
            break;
          case 2: // operator_binary_subtract_distribution_
          {
            OperatorSubtract operator_subtract(GenerateRandom(max_genarate_depth - 1),
                                               GenerateRandom(max_genarate_depth - 1));
            return std::make_shared<OperatorSubtract>(operator_subtract);
          }
            break;
          case 3: // operator_binary_divide_distribution_
          {
            OperatorDivide operator_divide(GenerateRandom(max_genarate_depth - 1),
                                           GenerateRandom(max_genarate_depth - 1));
            return std::make_shared<OperatorDivide>(operator_divide);
          }
            break;
          default:throw std::out_of_range("GenerateRandom operator_variant_binary out of range");
        }
      }
        break;
      case 1: // operator_type_unary_distribution_
      {
        std::discrete_distribution<int> distribution_unary{operator_unary_sma_distribution_,
                                                           operator_unary_ema_distribution_, operator_unary_atan_distribution_};
        unsigned long operator_variant_unary = distribution_unary(gen);
        switch (operator_variant_unary) {
          case 0: // operator_unary_sma_distribution_
          {
            double sma_coef = (double) rand() / RAND_MAX * max_coef;
            OperatorSma operator_sma(GenerateRandom(max_genarate_depth - 1), sma_coef);
            return std::make_shared<OperatorSma>(operator_sma);
          }
            break;
          case 1: // operator_unary_ema_distribution_
          {
            double ema_coef = ((double) rand() / RAND_MAX * max_coef);
            OperatorEma operator_ema(GenerateRandom(max_genarate_depth - 1), ema_coef);
            return std::make_shared<OperatorEma>(operator_ema);
          }
            break;
          case 2: // operator_unary_atan_distribution_
          {
            OperatorAtan operator_atan(GenerateRandom(max_genarate_depth - 1));
            return std::make_shared<OperatorAtan>(operator_atan);
          }
            break;
          default:throw std::out_of_range("GenerateRandom operator_variant_unary out of range");
        }
      }
        break;
      case 2: // operator_type_finite_distribution_
      {
        return GenerateFiniteRandom();
      }
        break;
      default:throw std::out_of_range("GenerateRandom operator_variant out of range");
    }
  } else {
    return GenerateFiniteRandom();
  }

  return return_operator;
}

std::shared_ptr<Operator> Generation::GenerateFiniteRandom(long operator_variant_param) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::discrete_distribution<int> distribution{operator_finite_series_distribution_, operator_finite_number_distribution_};
  unsigned long operator_variant = (operator_variant_param < 0) ? distribution(gen) : operator_variant_param;
  switch (operator_variant) {
    case 0: // operator_finite_series_distribution_
    {
      std::uniform_int_distribution<int> distribution_series(0, series_collection_.size() - 1);
      OperatorSeries series_generated(std::next(series_collection_.begin(), distribution_series(gen))->second);
      return std::make_shared<OperatorSeries>(series_generated);
    }
      break;
    case 1: // operator_finite_number_distribution_
    {
      OperatorNumber number_generated(rand() / (double) RAND_MAX * 2.0 * number_range - number_range);
      return std::make_shared<OperatorNumber>(number_generated);
    }
      break;
    default:throw std::out_of_range("GenerateFiniteRandom operator_variant out of range");
  }
}

void Generation::GenerateRandomSeed() {
  ThreadPool thread_pool(4);
  std::vector<std::future<std::pair<shared_ptr<Operator>, double>>> fitness_results;

  for (unsigned long i = 0; i < generation_count_; i++) {
    std::shared_ptr<Operator> operator_random = this->GenerateRandom();
    Operator::SimplifyOperator(operator_random);

    Trader local_trader(this->trader_);
    fitness_results.emplace_back(thread_pool.enqueue([operator_random, local_trader] {
      return std::pair<shared_ptr<Operator>,
                       double>(operator_random,
                               GetStrategyFitness(operator_random,
                                                  local_trader));
    }));
  }

  for (auto &&result: fitness_results) {
    this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(result.get()));
  }

  this->SortStrategy();
}

void Generation::SortStrategy() {
  sort(this->strategy_.begin(), this->strategy_.end(),
       [](std::pair<std::shared_ptr<Operator>, double> left, std::pair<std::shared_ptr<Operator>, double> right) ->
           bool { return (left.second > right.second); });

  RemoveGenerationDuplicates(this->strategy_);

  if (debug_output_) {
    cout << "Generation:" << endl;
    bool firstPass = true;
    for (auto &strategy : this->strategy_) {
      if (firstPass) {
        cout << strategy.first->ToString() << endl;
        cout << "---------------------------" << endl;
        firstPass = false;
        cout << strategy.second << endl;
      } else {
        //cout << strategy.first->ToString() << endl;
        //cout << strategy.second << endl;
        //cout << "+++++++++++++++++++++++++++" << endl;
      }
//            cout << strategy.second << endl;
    }
  }
}

std::pair<std::shared_ptr<Operator>, double> Generation::GetLeaderStrategy() {
  return std::pair<std::shared_ptr<Operator>, double>(*(this->strategy_.begin()));
}

std::pair<std::shared_ptr<Operator>, double> Generation::GetStrategy(unsigned long index) {
  return std::pair<std::shared_ptr<Operator>, double>(this->strategy_.at(index));
}

void Generation::UpdateStrategy(unsigned long index, pair<std::shared_ptr<Operator>, double> update_strategy) {
  if (this->strategy_.at(index).second < update_strategy.second) {
    this->strategy_.at(index).first = update_strategy.first;
    this->strategy_.at(index).second = update_strategy.second;
  }
}

void Generation::IterateGeneration(bool recalc) {

  std::random_device rd;
  std::mt19937 generator(rd());

  if (recalc) {
    vector<std::pair<std::shared_ptr<Operator>, double>> strategy_new;
    for (auto stategy : strategy_) {
      strategy_new.push_back(pair<std::shared_ptr<Operator>, double>(stategy.first,
                                                                     GetStrategyFitness(stategy.first, this->trader_)));
    }
    this->strategy_.clear();
    this->strategy_ = strategy_new;

    sort(this->strategy_.begin(), this->strategy_.end(),
         [](std::pair<std::shared_ptr<Operator>, double> left, std::pair<std::shared_ptr<Operator>, double> right) ->
             bool { return (left.second > right.second); });
  }

  while (this->strategy_.size() > generation_count_ / 2.0) {
    /*vector<double> distribution_selection;
    for (auto& key_val: this->strategy_) {
        distribution_selection.push_back(1.0 / (pow(key_val.second - this->strategy_.at(this->strategy_.size() - 1).second, 1.0) + 0.0000001));
    }
    std::discrete_distribution<> distribution_selection_variables(distribution_selection.begin(), distribution_selection.end());
    unsigned long i = distribution_selection_variables(generator);*/
    this->strategy_.pop_back();
  }

  vector<double> distribution;
  for (auto &key_val: this->strategy_) {
    distribution.push_back(pow(key_val.second - this->strategy_.at(this->strategy_.size() - 1).second, 2.0));
  }

  std::discrete_distribution<> distribution_variables(distribution.begin(), distribution.end());

  unsigned long initialSize = this->strategy_.size();
  ThreadPool thread_pool(4);
  std::vector<std::future<std::pair<shared_ptr<Operator>, double>>> fitness_results;

  for (unsigned long ii = 0; ii < max(initialSize / 2.0, generation_count_ / 2.0); ii++) {
    unsigned long i = distribution_variables(generator);
    std::shared_ptr<Operator> childOperator1 = std::shared_ptr<Operator>(this->strategy_.at(i).first->Clone());

    vector<double> distributionInit;
    for (unsigned long j = 0; j < initialSize; j++) {
      if (i != j) {
        distributionInit.push_back(
            (1.0 + pow(this->strategy_.at(j).second - this->strategy_.at(this->strategy_.size() - 1).second, 2.0))
                * log(2 * Operator::LevenshteinDistance(this->strategy_.at(j).first, this->strategy_.at(i).first) + 1)
                * pow(fabs(this->strategy_.at(i).second - this->strategy_.at(j).second), 2.0));
      } else {
        distributionInit.push_back(0.0);
      }
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::discrete_distribution<> distribution(distributionInit.begin(), distributionInit.end());
    unsigned long keySelected = distribution(generator);

    std::shared_ptr<Operator> childOperator2 = std::shared_ptr<Operator>(this->strategy_.at(keySelected).first->Clone());
    MakeCross(childOperator1, childOperator2);

    if (((double) rand() / (double) RAND_MAX) > 0.75) {
      GetCrossPoint(childOperator1, this->max_operator_depth_ - 1) =
          this->GenerateRandom(1 + floor((double) rand() / (double) RAND_MAX * this->max_operator_depth_));
    }
    Operator::SimplifyOperator(childOperator1);

    if (((double) rand() / (double) RAND_MAX) > 0.75) {
      GetCrossPoint(childOperator2, this->max_operator_depth_ - 1) =
          this->GenerateRandom(1 + floor((double) rand() / (double) RAND_MAX * this->max_operator_depth_));
    }
    Operator::SimplifyOperator(childOperator2);

    Trader local_trader(this->trader_);
    fitness_results.emplace_back(thread_pool.enqueue([childOperator1, local_trader] {
      return std::pair<shared_ptr<Operator>,
                       double>(childOperator1,
                               GetStrategyFitness(childOperator1,
                                                  local_trader));
    }));

    fitness_results.emplace_back(thread_pool.enqueue([childOperator2, local_trader] {
      return std::pair<shared_ptr<Operator>,
                       double>(childOperator2,
                               GetStrategyFitness(childOperator2,
                                                  local_trader));
    }));
  }

  for (auto &&result: fitness_results) {
    this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(result.get()));
  }

  this->SortStrategy();
}

double Generation::GetStrategyFitness(const std::shared_ptr<Operator> &strategy, const Trader &trader) {
  std::tuple<double, Series, Series, Series, Series, Series, Series> result = trader.Trade(strategy);
  return trader.CalculateTradeFitness(std::get<1>(result),
                                      std::get<2>(result),
                                      strategy);
}

void Generation::MakeCross(std::shared_ptr<Operator> &operator_parent_1, std::shared_ptr<Operator> &operator_parent_2,
                           unsigned long max_ganarate_depth) {
  std::shared_ptr<Operator> &operator_cross_1 = GetCrossPoint(operator_parent_1, max_ganarate_depth, max_ganarate_depth);
  std::shared_ptr<Operator> &operator_cross_2 = GetCrossPoint(operator_parent_2, max_ganarate_depth, max_ganarate_depth);
  operator_cross_1.swap(operator_cross_2);
}

std::shared_ptr<Operator> &Generation::GetCrossPoint(std::shared_ptr<Operator> &operator_parent,
                                                     unsigned long current_depth, unsigned long max_ganarate_depth) const {
  if (current_depth != max_ganarate_depth && ((double) rand() / RAND_MAX) > (0.5 * (double) current_depth / max_ganarate_depth)) {
    return operator_parent;
  }

  if (dynamic_cast<const OperatorBinary *>(operator_parent.get())) {
    if (((double) rand() / RAND_MAX) > 0.5) {
      return GetCrossPoint((dynamic_cast<OperatorBinary *>(operator_parent.get()))->GetOperatorLeft(),
                           --current_depth, max_ganarate_depth);
    } else {
      return GetCrossPoint((dynamic_cast<OperatorBinary *>(operator_parent.get()))->GetOperatorRight(),
                           --current_depth, max_ganarate_depth);
    }
  } else if (dynamic_cast<OperatorUnary *>(operator_parent.get())) {
    return GetCrossPoint((dynamic_cast<OperatorUnary *>(operator_parent.get()))->GetOperator(),
                         --current_depth, max_ganarate_depth);
  } else {
    return operator_parent;
  }
}

void Generation::RemoveGenerationDuplicates(vector<std::pair<std::shared_ptr<Operator>, double>> &strategy) {
  bool checkContinue = false;
  do {
    checkContinue = false;
    [&] {
      for (unsigned long ii = 0; ii < strategy.size(); ii++) {
        for (unsigned long ij = 0; ij < strategy.size(); ij++) {
          if (ii != ij && strategy.at(ii).first->ToString() == strategy.at(ij).first->ToString()) {
            strategy.erase(this->strategy_.begin() + ij);
            checkContinue = true;
            return;
          }
        }
      }
    }();
  } while (checkContinue);
}

void Generation::SetSeriesCollection(series_ptr_map_type series_collection) {
  this->series_collection_ = series_collection;
  for (auto strategy : this->strategy_) {
    strategy.first->RefreshSerieses(series_collection);
  }
}

/*void Generation::CutMaxOperatorDepth(const std::shared_ptr<Operator>& operator_parent, unsigned long max_ganarate_depth) {
    if (max_ganarate_depth > 0) {
        if (dynamic_cast<OperatorBinary*>(operator_parent.get())) {
            CutMaxOperatorDepth((*dynamic_cast<OperatorBinary*>(operator_parent.get())).GetOperatorLeft(),
                                       --max_ganarate_depth);
            CutMaxOperatorDepth((*dynamic_cast<OperatorBinary*>(operator_parent.get())).GetOperatorRight(),
                                --max_ganarate_depth);
        } else {
            CutMaxOperatorDepth((*dynamic_cast<OperatorUnary*>(operator_parent.get())).GetOperator(),
                                --max_ganarate_depth);
        }
    } else {

    }
}*/

