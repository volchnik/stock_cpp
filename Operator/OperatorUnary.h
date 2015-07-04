/* 
 * File:   OperatorUnary.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 9:25 PM
 */
#include "Operator.h"

#ifndef OPERATORUNARY_H
#define	OPERATORUNARY_H

class OperatorUnary : public Operator {
public:
    OperatorUnary(std::shared_ptr<Operator> operator_ptr, double coef);
    OperatorUnary(const OperatorUnary& orig);
    virtual ~OperatorUnary();
    
    virtual std::shared_ptr<Operator> perform() = 0;
protected:
    std::shared_ptr<Operator> operator_ptr_;
    double coef_;
};

#endif	/* OPERATORUNARY_H */

