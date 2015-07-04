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
    this->operatorPair_ = orig.operatorPair_;
}

OperatorBinary::~OperatorBinary() {
}

