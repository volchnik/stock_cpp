/* 
 * File:   OperatorUnary.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 9:25 PM
 */

#include "OperatorUnary.h"

OperatorUnary::OperatorUnary(std::shared_ptr<Operator> operator_ptr, double coef) {
    this->operator_ptr_ = operator_ptr;
    this->coef_ = coef;
}

OperatorUnary::OperatorUnary(const OperatorUnary& orig) {
    this->operator_ptr_ = orig.operator_ptr_;
}

OperatorUnary::~OperatorUnary() {
}

