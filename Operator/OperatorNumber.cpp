/* 
 * File:   OperatorNumber.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 10:15 PM
 */

#include "OperatorNumber.h"

OperatorNumber::OperatorNumber() {
}

OperatorNumber::OperatorNumber(double number) {
    this->number_ = number;
}

OperatorNumber::OperatorNumber(const OperatorNumber& orig) {
    this->number_ = orig.number_;
}

OperatorNumber::~OperatorNumber() {
}

OperatorNumber* OperatorNumber::Clone()
{
    return new OperatorNumber(*this);
}

double OperatorNumber::getNumber() {
    return this->number_;
}

std::string OperatorNumber::ToString() {
    return std::string(std::to_string(number_));
}
