/* 
 * File:   OperatorUnary.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 9:25 PM
 */
#include "Operator.h"
#include "OperatorNumber.h"

#ifndef OPERATORUNARY_H
#define	OPERATORUNARY_H

class OperatorUnary : public Operator {
public:
    OperatorUnary(std::shared_ptr<Operator> operator_ptr, std::shared_ptr<OperatorNumber> coef);
    OperatorUnary(const OperatorUnary& orig);
    virtual ~OperatorUnary();
    
    virtual OperatorUnary* Clone() const = 0;
    
    virtual std::shared_ptr<Operator> perform() const = 0;
    virtual std::string ToString() const = 0;
    
    std::shared_ptr<Operator>& GetOperator();
    std::shared_ptr<OperatorNumber>& GetCoef();
    
    enum operator_type {SMA, EMA, ATAN};

    void RefreshSerieses(map<std::string, std::shared_ptr<Series>> collection);
protected:
    std::shared_ptr<Operator> operator_ptr_;
    std::shared_ptr<OperatorNumber> coef_;
};

#endif	/* OPERATORUNARY_H */

