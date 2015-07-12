/* 
 * File:   OperatorUnary.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 9:25 PM
 */

#include "OperatorUnary.h"

OperatorUnary::OperatorUnary(std::shared_ptr<Operator> operator_ptr, double coef) :
    operator_ptr_(operator_ptr), coef_(coef) {
}

OperatorUnary::OperatorUnary(const OperatorUnary& orig) {
    this->operator_ptr_ = orig.operator_ptr_;
    this->coef_ = orig.coef_;
}

OperatorUnary::~OperatorUnary() {
}

