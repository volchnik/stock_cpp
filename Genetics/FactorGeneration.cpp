//
//  FactorGeneration.cpp
//  stock_cpp
//
//  Created by NIKITA VOLCHEGORSKIY on 11/02/16.
//  Copyright © 2016 NIKITA VOLCHEGORSKIY. All rights reserved.
//
#include "FactorGeneration.h"
#include "../ThreadPool.h"
#include "../Genetics/Generation.h"
#include "../Series.h"
#include "../Operator/OperatorNumber.h"
#include "../Operator/OperatorSeries.h"
#include "../Operator/OperatorSma.h"
#include "../Operator/OperatorEma.h"
#include "../Operator/OperatorMultiply.h"
#include "../Operator/OperatorAdd.h"
#include "../Operator/OperatorSubtract.h"
#include "../Operator/OperatorDivide.h"

FactorGeneration::FactorGeneration(std::shared_ptr<Operator> initial_operator, double initial_score, unsigned long generation_capacity, Trader trader) : initial_operator_(initial_operator), initial_score_(initial_score), generation_capacity_(generation_capacity), trader_(trader), generation_count_(0) {
    FindOperatorFactors(this->initial_operator_, this->operator_factors_);
}

FactorGeneration::~FactorGeneration() {
}

void FactorGeneration::FindOperatorFactors(std::shared_ptr<Operator> operator_initial,
                                vector<shared_ptr<OperatorNumber>>& operator_factors) {
    if (dynamic_cast<const OperatorBinary*>(operator_initial.get())) {
        FindOperatorFactors((dynamic_cast<OperatorBinary*>(operator_initial.get()))->GetOperatorLeft(), operator_factors);
        FindOperatorFactors((dynamic_cast<OperatorBinary*>(operator_initial.get()))->GetOperatorRight(), operator_factors);
    } else if (dynamic_cast<OperatorUnary*>(operator_initial.get())) {
        operator_factors.push_back(std::dynamic_pointer_cast<OperatorNumber>((
            dynamic_cast<OperatorUnary*>(operator_initial.get()))->GetCoef()));
        FindOperatorFactors((dynamic_cast<OperatorUnary*>(operator_initial.get()))->GetOperator(), operator_factors);
    } else if (dynamic_cast<OperatorNumber*>(operator_initial.get())) {
        operator_factors.push_back(std::dynamic_pointer_cast<OperatorNumber>(operator_initial));
    }
}

void FactorGeneration::GenerateRandomSeed() {
    if (operator_factors_.size() == 0) {
        return;
    }

    vector<double> initial_factors;
    for (shared_ptr<OperatorNumber> operator_num : operator_factors_) {
        initial_factors.push_back(operator_num->getNumber());
    }
    
    ThreadPool thread_pool(4);
    std::vector<std::future<std::pair<vector<double>, double>>> fitness_results;
    
    factor_generation_.push_back(pair<vector<double>, double>(initial_factors, initial_score_));

    for (unsigned long i = 1; i < generation_capacity_; i++) {
        vector<double> factors;
        for (unsigned long j = 0; j < initial_factors.size(); j++) {
            double coef = initial_factors.at(j) + seed_range_ * (((double)rand() / RAND_MAX) - 0.5) * (1.0 / (1.0 + pow(generation_count_, 1.5)));
            factors.push_back(coef);
            operator_factors_.at(j)->setNumber(coef);
        }
        
        Trader local_trader(this->trader_);
        std::shared_ptr<Operator> operator_local(initial_operator_);
        fitness_results.emplace_back(thread_pool.enqueue([factors, operator_local, local_trader]{ return std::pair<vector<double>, double>(factors, Generation::GetStrategyFitness(operator_local, local_trader)); }));
    }
    
    for(auto && result: fitness_results) {
        factor_generation_.push_back(std::pair<vector<double>, double>(result.get()));
    }
    
    SortGeneration();
}

double FactorGeneration::IterateGeneration() {
    if (operator_factors_.size() == 0) {
        return initial_score_;
    }

    while (factor_generation_.size() > generation_capacity_ / 2.0 ||
           (factor_generation_.size()  % 2 != 0)) {
        factor_generation_.pop_back();
    }
    
    unsigned long initialSize = factor_generation_.size();
    ThreadPool thread_pool(4);
    std::vector<std::future<std::pair<vector<double>, double>>> fitness_results;
    
    for (unsigned long ii = 0; ii < initialSize; ii++) {
        vector<double> childOperator1 = factor_generation_.at(ii).first;
        vector<double> childOperator2 = factor_generation_.at((long)(floor(ii + ((double)rand() / RAND_MAX) * initialSize)) % initialSize).first;
        
        unsigned long crossingover_offset = floor(((double)rand() / RAND_MAX) * childOperator1.size());
        vector<double> childOperator3;
        
        for (unsigned long j = 0; j < childOperator1.size(); j++) {
            double coef = 0.0;
            if (j <= crossingover_offset) {
                coef = childOperator1.at(j);
            } else {
                coef = childOperator2.at(j);
            }
            
            if (((double)rand() / RAND_MAX) < 0.25) {
                coef += + seed_range_ * (((double)rand() / RAND_MAX) - 0.5) * (1.0 / (1.0 + pow(generation_count_, 1.5)));
            }
            childOperator3.push_back(coef);
            operator_factors_.at(j)->setNumber(coef);
        }
        
        Trader local_trader(this->trader_);
        std::shared_ptr<Operator> operator_local = std::shared_ptr<Operator>(initial_operator_->Clone());
        fitness_results.emplace_back(thread_pool.enqueue([childOperator3, operator_local, local_trader]{ return std::pair<vector<double>, double>(childOperator3, Generation::GetStrategyFitness(operator_local, local_trader)); }));
    }
    
    for(auto && result: fitness_results) {
        factor_generation_.push_back(std::pair<vector<double>, double>(result.get()));
    }
    
    return SortGeneration();
}

double FactorGeneration::SortGeneration() {
    sort(this->factor_generation_.begin(), this->factor_generation_.end(),
     [](std::pair<vector<double>, double> left, std::pair<vector<double>, double> right) ->
     bool{ return (left.second > right.second);});
    
    for (unsigned long j = 0; j < factor_generation_.begin()->first.size(); j++) {
        operator_factors_.at(j)->setNumber(factor_generation_.begin()->first.at(j));
    }
    generation_count_++;
    
    return factor_generation_.begin()->second;
}