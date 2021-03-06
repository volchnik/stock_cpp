/* 
 * File:   OperatorUnary.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 9:25 PM
 */

#include "OperatorUnary.h"

OperatorUnary::OperatorUnary(std::shared_ptr<Operator> operator_ptr, std::shared_ptr<OperatorNumber> coef) :
    operator_ptr_(operator_ptr), coef_(coef) {
}

OperatorUnary::OperatorUnary(const OperatorUnary& orig) {
    this->operator_ptr_ = std::shared_ptr<Operator>(orig.operator_ptr_->Clone());
    this->coef_ = std::shared_ptr<OperatorNumber>(orig.coef_->Clone());
}

OperatorUnary::~OperatorUnary() {
}

std::shared_ptr<Operator>& OperatorUnary::GetOperator() {
    return this->operator_ptr_;
}

std::shared_ptr<OperatorNumber>& OperatorUnary::GetCoef() {
    return this->coef_;
}

void OperatorUnary::RefreshSerieses(map<std::string, std::shared_ptr<Series>> collection) {
    operator_ptr_->RefreshSerieses(collection);
}