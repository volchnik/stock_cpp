/* 
 * File:   Generation.h
 * Author: volchnik
 *
 * Created on June 7, 2015, 5:16 PM
 */
#include "../root.h"
#include "../Series.h"
#include "../Operator/Operator.h"
#include "Trader.h"

#ifndef GENERATION_H
#define	GENERATION_H

class Generation {
public:
    Generation(const unsigned long& generation_count, const vector<std::shared_ptr<Series>>& series_collection, Trader trader);
    Generation(const Generation& orig);
    virtual ~Generation();
    
    static const unsigned long max_operator_depth_ = 5;
    
    static const unsigned long operator_type_binary_distribution_ = 3;
    static const unsigned long operator_type_unary_distribution_ = 2;
    static const unsigned long operator_type_finite_distribution_ = 1;
    
    static const unsigned long operator_binary_multiply_distribution_ = 1;
    static const unsigned long operator_binary_add_distribution_ = 1;
    static const unsigned long operator_binary_subtract_distribution_ = 1;
    static const unsigned long operator_binary_divide_distribution_ = 1;
    
    static const unsigned long operator_unary_sma_distribution_ = 1;
    static const unsigned long operator_unary_ema_distribution_ = 2;
    
    static const unsigned long operator_finite_series_distribution_ = 1;
    static const unsigned long operator_finite_number_distribution_ = 1;
    
    static constexpr double number_range = 100;
    static constexpr double max_coef = 100;
    
    std::shared_ptr<Operator> GenerateRandom(unsigned long max_ganarate_depth = max_operator_depth_);
    
    void GenerateRandomSeed();
    void IterateGeneration();
    
    double GetStrategyFitness(const std::shared_ptr<Operator>& strategy);
    
    // Простое скрещивание операторов
    void MakeCross(std::shared_ptr<Operator>& operator_parent_1, std::shared_ptr<Operator>& operator_parent_2, unsigned long max_ganarate_depth = max_operator_depth_);

protected:
    std::shared_ptr<Operator> GenerateFiniteRandom();
    std::shared_ptr<Operator>& GetCrossPoint(std::shared_ptr<Operator>& operator_parent, unsigned long current_depth = max_operator_depth_,
                                            unsigned long max_ganarate_depth = max_operator_depth_);
    //void CutMaxOperatorDepth(const std::shared_ptr<Operator>& operator_parent, unsigned long max_ganarate_depth);
    void SortStrategy();
    
private:
    vector<std::pair<std::shared_ptr<Operator>, double>> strategy_;
    unsigned long generation_count_;
    vector<std::shared_ptr<Series>> series_collection_;
    Trader trader_;
};

#endif	/* GENERATION_H */

