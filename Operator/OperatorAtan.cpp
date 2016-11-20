//
//  OperatorAtan.cpp
//  stock_cpp
//
//  Created by NIKITA VOLCHEGORSKIY on 28/06/16.
//  Copyright © 2016 NIKITA VOLCHEGORSKIY. All rights reserved.
//

#include "OperatorAtan.h"
#include "OperatorSeries.h"

OperatorAtan::OperatorAtan(std::shared_ptr<Operator> operator_ptr) :
OperatorUnary(operator_ptr, make_shared<OperatorNumber>(0)) {
}

OperatorAtan::OperatorAtan(const OperatorAtan& orig) : OperatorUnary(orig) {
}

OperatorAtan::~OperatorAtan() {
}

OperatorAtan* OperatorAtan::Clone() const
{
    return new OperatorAtan(*this);
}

std::shared_ptr<Operator> OperatorAtan::perform() const {
    std::shared_ptr<Operator> operator_ptr = this->operator_ptr_->perform();
    if (dynamic_cast<OperatorSeries*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorSeries>(std::make_shared<Series>(
                                                                         dynamic_cast<OperatorSeries*> (operator_ptr.get())->getSeries()->AtanIndicator()));
    } else if (dynamic_cast<OperatorNumber*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorNumber>(atan(dynamic_cast<OperatorNumber*> (operator_ptr.get())->getNumber()) / M_PI_2);
    } else {
        throw std::invalid_argument("wrong pointer type");
    }
}

std::string OperatorAtan::ToString() const {
    return std::string("ATAN(" + operator_ptr_->ToString() + ")");
}