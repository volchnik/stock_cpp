/* 
 * File:   OperatorSma.cpp
 * Author: volchnik
 * 
 * Created on June 6, 2015, 1:18 AM
 */

#include "OperatorSma.h"
#include "OperatorSeries.h"

OperatorSma::OperatorSma(std::shared_ptr<Operator> operator_ptr, double coef) :
    OperatorUnary(operator_ptr, coef) {
}

OperatorSma::OperatorSma(const OperatorSma& orig) : OperatorUnary(orig) {
}

OperatorSma::~OperatorSma() {
}

std::shared_ptr<Operator> OperatorSma::perform() {
    std::shared_ptr<Operator> operator_ptr = this->operator_ptr_->perform();
    return std::make_shared<OperatorSeries>(std::make_shared<Series>(
            dynamic_cast<OperatorSeries*>(operator_ptr.get())->getSeries()->SmaIndicator(coef_)));
}

