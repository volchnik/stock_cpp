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
    
    virtual OperatorUnary* Clone() = 0;
    
    virtual std::shared_ptr<Operator> perform() = 0;
    virtual std::string ToString() = 0;
    
    std::shared_ptr<Operator>& GetOperator();
    void SetOperator(std::shared_ptr<Operator>& operator_param);
protected:
    std::shared_ptr<Operator> operator_ptr_;
    double coef_;
};

#endif	/* OPERATORUNARY_H */

