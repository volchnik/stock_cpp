/* 
 * File:   OperatorEma.cpp
 * Author: volchnik
 * 
 * Created on May 24, 2015, 11:22 PM
 */

#include "OperatorEma.h"
#include "OperatorSeries.h"

OperatorEma::OperatorEma(std::shared_ptr<Operator> operator_ptr, double coef) :
    OperatorUnary(operator_ptr, coef) {
}

OperatorEma::OperatorEma(const OperatorEma& orig) : OperatorUnary(orig) {
}

OperatorEma::~OperatorEma() {
}

std::shared_ptr<Operator> OperatorEma::perform() {
    std::shared_ptr<Operator> operator_ptr = this->operator_ptr_->perform();
    return std::make_shared<OperatorSeries>( std::make_shared<Series>(
            dynamic_cast<OperatorSeries*>(operator_ptr.get())->getSeries()->EmaIndicator(coef_)));
}

