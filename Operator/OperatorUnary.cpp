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
    this->operator_ptr_ = std::shared_ptr<Operator>(orig.operator_ptr_->Clone());
    this->coef_ = orig.coef_;
}

OperatorUnary::~OperatorUnary() {
}

std::shared_ptr<Operator>& OperatorUnary::GetOperator() {
    return this->operator_ptr_;
}

void OperatorUnary::SetOperator(std::shared_ptr<Operator>& operator_param) {
    this->operator_ptr_ = operator_param;
}