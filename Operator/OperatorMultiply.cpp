/* 
 * File:   OperatorMultiply.cpp
 * Author: volchnik
 * 
 * Created on June 6, 2015, 10:10 AM
 */
#include "OperatorSeries.h"
#include "OperatorNumber.h"
#include "OperatorMultiply.h"

OperatorMultiply::OperatorMultiply(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second) : 
    OperatorBinary(first, second) {
}

OperatorMultiply::OperatorMultiply(const OperatorMultiply& orig) : OperatorBinary(orig) {
}

OperatorMultiply::~OperatorMultiply() {
}

OperatorMultiply* OperatorMultiply::Clone()
{
    return new OperatorMultiply(*this);
}

std::shared_ptr<Operator> OperatorMultiply::perform() {
    
    std::shared_ptr<Operator> first_ptr = this->operatorPair_.first->perform();
    std::shared_ptr<Operator> second_ptr = this->operatorPair_.second->perform();

    if (typeid(*first_ptr) != typeid(OperatorNumber) ||
            typeid(*second_ptr) != typeid(OperatorNumber)) {
        std::shared_ptr<Series> returnSeriesPtr = std::make_shared<Series>(
                (typeid(*first_ptr) == typeid(OperatorSeries)) ? 
                    ((typeid(*second_ptr) == typeid(OperatorSeries)) ? 
                        (*dynamic_cast<OperatorSeries*>(first_ptr.get())->getSeries() * 
                        *dynamic_cast<OperatorSeries*>(second_ptr.get())->getSeries()) :
                        (*dynamic_cast<OperatorSeries*>(first_ptr.get())->getSeries() * 
                        dynamic_cast<OperatorNumber*>(second_ptr.get())->getNumber())) :
                    (dynamic_cast<OperatorNumber*>(first_ptr.get())->getNumber() * 
                        *dynamic_cast<OperatorSeries*>(second_ptr.get())->getSeries()));

        return std::make_shared<OperatorSeries> (returnSeriesPtr);
    } else {
        return std::make_shared<OperatorNumber> (
        dynamic_cast<OperatorNumber*>(first_ptr.get())->getNumber() * 
        dynamic_cast<OperatorNumber*>(second_ptr.get())->getNumber());
    }    
}

std::string OperatorMultiply::ToString() {
    return std::string("(" + operatorPair_.first->ToString() + " * " + operatorPair_.second->ToString() + ")");
}
