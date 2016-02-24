/* 
 * File:   OperatorBinary.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 10:12 PM
 */
#include "Operator.h"

#ifndef OPERATORBINARY_H
#define	OPERATORBINARY_H

class OperatorBinary : public Operator {
public:
    OperatorBinary(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second);
    OperatorBinary(const OperatorBinary& orig);
    virtual ~OperatorBinary();
    
    virtual OperatorBinary* Clone() const = 0;
    
    virtual std::shared_ptr<Operator> perform() const = 0;
    virtual std::string ToString() const = 0;
    
    std::shared_ptr<Operator>& GetOperatorLeft();
    std::shared_ptr<Operator>& GetOperatorRight();
    void SetOperatorLeft(const std::shared_ptr<Operator>& operator_param);
    void SetOperatorRight(const std::shared_ptr<Operator>& operator_param);
    
    enum operator_type {ADD, SUBTRACT, MULITPLAY, DIVIDE};
protected:
    std::pair<std::shared_ptr<Operator>, std::shared_ptr<Operator>> operatorPair_;
};

#endif	/* OPERATORBINARY_H */

