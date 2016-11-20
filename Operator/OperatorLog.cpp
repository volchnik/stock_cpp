//
//  OperatorLog.cpp
//  stock_cpp
//
//  Created by NIKITA VOLCHEGORSKIY on 16/11/16.
//  Copyright © 2016 NIKITA VOLCHEGORSKIY. All rights reserved.
//

#include "OperatorLog.h"
#include "OperatorSeries.h"

OperatorLog::OperatorLog(std::shared_ptr<Operator> operator_ptr) :
OperatorUnary(operator_ptr, make_shared<OperatorNumber>(0)) {
}

OperatorLog::OperatorLog(const OperatorLog& orig) : OperatorUnary(orig) {
}

OperatorLog::~OperatorLog() {
}

OperatorLog* OperatorLog::Clone() const
{
    return new OperatorLog(*this);
}

std::shared_ptr<Operator> OperatorLog::perform() const {
    std::shared_ptr<Operator> operator_ptr = this->operator_ptr_->perform();
    if (dynamic_cast<OperatorSeries*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorSeries>(std::make_shared<Series>(
                dynamic_cast<OperatorSeries*> (operator_ptr.get())->getSeries()->LogIndicator()));
    } else if (dynamic_cast<OperatorNumber*> (operator_ptr.get()) != NULL) {
        return std::make_shared<OperatorNumber>(log10(dynamic_cast<OperatorNumber*> (operator_ptr.get())->getNumber() + 1.0));
    } else {
        throw std::invalid_argument("wrong pointer type");
    }
}

std::string OperatorLog::ToString() const {
    return std::string("LOG(" + operator_ptr_->ToString() + ")");
}
