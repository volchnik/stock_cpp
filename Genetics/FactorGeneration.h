//
//  FactorGeneration.h
//  stock_cpp
//
//  Created by NIKITA VOLCHEGORSKIY on 11/02/16.
//  Copyright © 2016 NIKITA VOLCHEGORSKIY. All rights reserved.
//

#include "../Operator/Operator.h"
#include "../Operator/OperatorNumber.h"
#include "../Trader.h"

#ifndef FactorGeneration_h
#define FactorGeneration_h

class FactorGeneration {
public:
    FactorGeneration(std::shared_ptr<Operator> initial_operator, double score, unsigned long generation_capacity, Trader trader);
    virtual ~FactorGeneration();

    static void FindOperatorFactors(std::shared_ptr<Operator> operator_initial,
                                    vector<shared_ptr<OperatorNumber>>& operator_factors);
    
    void GenerateRandomSeed();
    double IterateGeneration();
protected:
    double SortGeneration();
private:
    shared_ptr<Operator> initial_operator_;
    double initial_score_;
    unsigned long generation_capacity_;
    vector<shared_ptr<OperatorNumber>> operator_factors_;
    vector<pair<vector<double>, double>> factor_generation_;
    
    unsigned long generation_count_;
    Trader trader_;
};

#endif /* FactorGeneration_h */
