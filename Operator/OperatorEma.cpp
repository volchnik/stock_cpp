/* 
 * File:   OperatorEma.cpp
 * Author: volchnik
 * 
 * Created on May 24, 2015, 11:22 PM
 */

#include "OperatorEma.h"
#include "OperatorSeries.h"
#include "OperatorNumber.h"

OperatorEma::OperatorEma(std::shared_ptr<Operator> operator_ptr, double coef) :
    OperatorUnary(operator_ptr, make_shared<OperatorNumber>(coef)) {
}

OperatorEma::OperatorEma(const OperatorEma& orig) : OperatorUnary(orig) {
}

OperatorEma::~OperatorEma() {
}

OperatorEma* OperatorEma::Clone() const
{
    return new OperatorEma(*this);
}

std::shared_ptr<Operator> OperatorEma::perform() const {
    std::shared_ptr<Operator> operator_ptr = this->operator_ptr_->perform();
    if (dynamic_cast<OperatorSeries*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorSeries>(std::make_shared<Series>(
                dynamic_cast<OperatorSeries*> (operator_ptr.get())->getSeries()->EmaIndicator(coef_->getNumber())));
    } else if (dynamic_cast<OperatorNumber*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorNumber>(dynamic_cast<OperatorNumber*> (operator_ptr.get())->getNumber());
    } else {
        throw std::invalid_argument("wrong pointer type");
    }
}

std::string OperatorEma::ToString() const {
    return std::string("EMA(" + operator_ptr_->ToString() + ", " + std::to_string(coef_->getNumber()) + ")");
}

