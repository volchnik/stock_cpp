/* 
 * File:   OperatorNumber.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 10:15 PM
 */
#include "Operator.h"

#ifndef OPERATORNUMBER_H
#define	OPERATORNUMBER_H

class OperatorNumber : public Operator {
public:
    OperatorNumber();
    OperatorNumber(double number);
    OperatorNumber(const OperatorNumber& orig);
    virtual ~OperatorNumber();
    
    OperatorNumber* Clone() const;
    
    std::shared_ptr<Operator> perform() const { return std::make_shared<OperatorNumber>(*this); };
    double getNumber();
    void setNumber(double number) {
        this->number_ = number;
    }
    std::string ToString() const;
private:
    double number_;
};

#endif	/* OPERATORNUMBER_H */

