/* 
 * File:   OperatorSma.cpp
 * Author: volchnik
 * 
 * Created on June 6, 2015, 1:18 AM
 */

#include "OperatorSma.h"
#include "OperatorSeries.h"
#include "OperatorNumber.h"

OperatorSma::OperatorSma(std::shared_ptr<Operator> operator_ptr, double coef) :
    OperatorUnary(operator_ptr, coef) {
}

OperatorSma::OperatorSma(const OperatorSma& orig) : OperatorUnary(orig) {
}

OperatorSma::~OperatorSma() {
}

std::shared_ptr<Operator> OperatorSma::perform() {
    std::shared_ptr<Operator> operator_ptr = this->operator_ptr_->perform();
    if (dynamic_cast<OperatorSeries*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorSeries>(std::make_shared<Series>(
                dynamic_cast<OperatorSeries*> (operator_ptr.get())->getSeries()->SmaIndicator(coef_)));
    } else if (dynamic_cast<OperatorNumber*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorNumber>(dynamic_cast<OperatorNumber*> (operator_ptr.get())->getNumber());
    } else {
        throw std::invalid_argument("wrong pointer type");
    }
}

std::string OperatorSma::ToString() {
    return std::string("SMA(" + operator_ptr_->ToString() + ", " + std::to_string(coef_) + ")");
}
