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

Generation::Generation(const unsigned long& generation_count, const vector<std::shared_ptr<Series>>& series_collection) 
        : generation_count_(generation_count), series_collection_(series_collection) {
}

Generation::Generation(const Generation& orig) : strategy_(orig.strategy_), generation_count_(orig.generation_count_) {
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

