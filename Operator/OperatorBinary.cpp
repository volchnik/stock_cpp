/* 
 * File:   OperatorBinary.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 10:12 PM
 */

#include "OperatorBinary.h"

OperatorBinary::OperatorBinary(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second) {
    this->operatorPair_.first = first;
    this->operatorPair_.second = second;
}

OperatorBinary::OperatorBinary(const OperatorBinary& orig) {
    this->operatorPair_.first = std::shared_ptr<Operator>(orig.operatorPair_.first->Clone());
    this->operatorPair_.second = std::shared_ptr<Operator>(orig.operatorPair_.second->Clone());
}

OperatorBinary::~OperatorBinary() {
}

std::shared_ptr<Operator>& OperatorBinary::GetOperatorLeft() {
    return this->operatorPair_.first;
}

std::shared_ptr<Operator>& OperatorBinary::GetOperatorRight() {
    return this->operatorPair_.second;
}

void OperatorBinary::SetOperatorLeft(const std::shared_ptr<Operator>& operator_param) {
    this->operatorPair_.first = operator_param;
}

void OperatorBinary::SetOperatorRight(const std::shared_ptr<Operator>& operator_param) {
    this->operatorPair_.second = operator_param;
}

void OperatorBinary::RefreshSerieses(map<std::string, std::shared_ptr<Series>> collection) {
    operatorPair_.first->RefreshSerieses(collection);
    operatorPair_.second->RefreshSerieses(collection);
}

