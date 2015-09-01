/* 
 * File:   Generation.cpp
 * Author: volchnik
 * 
 * Created on June 7, 2015, 5:16 PM
 */

#include "Generation.h"
#include "../Series.h"
#include "../Operator/OperatorNumber.h"
#include "../Operator/OperatorSeries.h"
#include "../Operator/OperatorSma.h"
#include "../Operator/OperatorEma.h"
#include "../Operator/OperatorMultiply.h"
#include "../Operator/OperatorAdd.h"
#include "../Operator/OperatorSubtract.h"
#include "../Operator/OperatorDivide.h"

Generation::Generation(const unsigned long& generation_count, const vector<std::shared_ptr<Series>>& series_collection, Trader trader)
        : generation_count_(generation_count), series_collection_(series_collection), trader_(trader) {
}

Generation::Generation(const Generation& orig) : strategy_(orig.strategy_), generation_count_(orig.generation_count_), trader_(orig.trader_) {
}

Generation::~Generation() {
}

std::shared_ptr<Operator> Generation::GenerateRandom(unsigned long max_ganarate_depth) {
    std::shared_ptr<Operator> return_operator;

    if (max_ganarate_depth > 1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<int> distribution{operator_type_binary_distribution_,
            operator_type_unary_distribution_, operator_type_finite_distribution_};
        unsigned long operator_variant = distribution(gen);
        switch (operator_variant) {
            case 0: // operator_type_binary_distribution_
            {
                std::discrete_distribution<int> distribution_binary{operator_binary_multiply_distribution_,
                    operator_binary_add_distribution_, operator_binary_subtract_distribution_, operator_binary_divide_distribution_};
                unsigned long operator_variant_binary = distribution_binary(gen);
                switch (operator_variant_binary) {
                    case 0: // operator_binary_multiply_distribution_
                    {
                        OperatorMultiply operator_multiply(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorMultiply>(operator_multiply);
                    }
                        break;
                    case 1: // operator_binary_add_distribution_
                    {
                        OperatorAdd operator_add(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorAdd>(operator_add);
                    }
                        break;
                    case 2: // operator_binary_subtract_distribution_
                    {
                        OperatorSubtract operator_subtract(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorSubtract>(operator_subtract);
                    }
                        break;
                    case 3: // operator_binary_divide_distribution_
                    {
                        OperatorDivide operator_divide(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorDivide>(operator_divide);
                    }
                        break;
                    default:
                        throw std::out_of_range("GenerateRandom operator_variant_binary out of range");
                }
            }
                break;
            case 1: // operator_type_unary_distribution_
            {
                std::discrete_distribution<int> distribution_unary{operator_unary_sma_distribution_, operator_unary_ema_distribution_};
                unsigned long operator_variant_unary = distribution_unary(gen);
                switch (operator_variant_unary) {
                    case 0: // operator_unary_sma_distribution_
                    {
                        double sma_coef = ((double) rand() / RAND_MAX * max_coef);
                        OperatorSma operator_sma(GenerateRandom(max_ganarate_depth - 1), sma_coef);
                        return std::make_shared<OperatorSma>(operator_sma);
                    }
                        break;
                    case 1: // operator_unary_ema_distribution_
                    {
                        double ema_coef = ((double) rand() / RAND_MAX * max_coef);
                        OperatorEma operator_ema(GenerateRandom(max_ganarate_depth - 1), ema_coef);
                        return std::make_shared<OperatorEma>(operator_ema);
                    }
                        break;
                    default:
                        throw std::out_of_range("GenerateRandom operator_variant_unary out of range");
                }
            }
                break;
            case 2: // operator_type_finite_distribution_
            {
                return GenerateFiniteRandom();
            }
                break;
            default:
                throw std::out_of_range("GenerateRandom operator_variant out of range");
        }
    } else {
        return GenerateFiniteRandom();
    }
    
    return return_operator;
}

std::shared_ptr<Operator> Generation::GenerateFiniteRandom() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution{operator_finite_series_distribution_, operator_finite_number_distribution_};
    unsigned long operator_variant = distribution(gen);
    switch (operator_variant) {
        case 0: // operator_finite_series_distribution_
        {
            std::uniform_int_distribution<int> distribution_series(0, series_collection_.size() - 1);
            OperatorSeries series_generated(series_collection_.at(distribution_series(gen)));
            return std::make_shared<OperatorSeries>(series_generated);
        }
            break;
        case 1: // operator_finite_number_distribution_
        {
            OperatorNumber number_generated(rand() / (double) RAND_MAX * 2.0 * number_range - number_range);
            return std::make_shared<OperatorNumber>(number_generated);
        }
            break;
        default:
            throw std::out_of_range("GenerateFiniteRandom operator_variant out of range");
    }
}

void Generation::GenerateRandomSeed() {
    for (unsigned long i = 0; i < generation_count_; i++) {
        std::shared_ptr<Operator> operator_random = this->GenerateRandom();
        double operator_quality = GetStrategyFitness(operator_random);
        this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(operator_random, operator_quality));
    }
    this->SortStrategy();
}

void Generation::SortStrategy() {
    sort(this->strategy_.begin(), this->strategy_.end(),
         [](std::pair<std::shared_ptr<Operator>, double> left, std::pair<std::shared_ptr<Operator>, double> right) ->
         bool{ return (left.second > right.second);});
    
    cout << "Sort:" << endl;
    bool firstPass = true;
    for (auto& strategy : this->strategy_) {
        if (firstPass) {
            cout << strategy.first->ToString() << endl;
            cout << "---------------------------" << endl;
            firstPass = false;
        }
        cout << strategy.second << endl;
    }
}

void Generation::IterateGeneration() {
    while (this->strategy_.size() > generation_count_ / 2.0 ||
           (this->strategy_.size() % 2 != 0)) {
        this->strategy_.pop_back();
    }
    
    unsigned long initialSize = this->strategy_.size();
    for (unsigned long i = 0; i < initialSize; i+=2) {
        std::shared_ptr<Operator> childOperator1 = std::shared_ptr<Operator>(this->strategy_.at(i).first->Clone());
        std::shared_ptr<Operator> childOperator2 = std::shared_ptr<Operator>(this->strategy_.at(i + 1).first->Clone());
        MakeCross(childOperator1, childOperator2);
        GetCrossPoint(childOperator1) = this->GenerateRandom();
        double childOperator_1_quality = GetStrategyFitness(childOperator1);
        this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(childOperator1, childOperator_1_quality));
        GetCrossPoint(childOperator2) = this->GenerateRandom();
        double childOperator_2_quality = GetStrategyFitness(childOperator2);
        this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(childOperator2, childOperator_2_quality));
    }
    
    this->SortStrategy();
}

double Generation::GetStrategyFitness(const std::shared_ptr<Operator>& strategy) {
    std::tuple<double, Series, Series, Series, Series> result = trader_.Trade(strategy);
    Series trade_account = std::get<2>(result);
    return trade_account.CalculateBoundStatistic();
}

void Generation::MakeCross(std::shared_ptr<Operator>& operator_parent_1, std::shared_ptr<Operator>& operator_parent_2,
                           unsigned long max_ganarate_depth) {
    GetCrossPoint(operator_parent_1, max_ganarate_depth, max_ganarate_depth).
        swap(GetCrossPoint(operator_parent_2, max_ganarate_depth, max_ganarate_depth));
}

std::shared_ptr<Operator>& Generation::GetCrossPoint(std::shared_ptr<Operator>& operator_parent,
                            unsigned long current_depth, unsigned long max_ganarate_depth) {
    if (current_depth != max_ganarate_depth && ((double) rand() / RAND_MAX) > (1.0 - ((double) current_depth / max_ganarate_depth))) {
        return operator_parent;
    }
    
    if (dynamic_cast<OperatorBinary*>(operator_parent.get())) {
        if (((double) rand() / RAND_MAX) > 0.5) {
            return GetCrossPoint((*dynamic_cast<OperatorBinary*>(operator_parent.get())).GetOperatorLeft(),
                    --current_depth, max_ganarate_depth);
        } else {
            return GetCrossPoint((*dynamic_cast<OperatorBinary*>(operator_parent.get())).GetOperatorRight(),
                                 --current_depth, max_ganarate_depth);
        }
    } else if (dynamic_cast<OperatorUnary*>(operator_parent.get())) {
        return GetCrossPoint((*dynamic_cast<OperatorUnary*>(operator_parent.get())).GetOperator(),
                             --current_depth, max_ganarate_depth);
    } else {
        return operator_parent;
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

