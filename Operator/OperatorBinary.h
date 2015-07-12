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
    
    virtual std::shared_ptr<Operator> perform() = 0;
    virtual std::string ToString() = 0;
protected:
    std::pair<std::shared_ptr<Operator>, std::shared_ptr<Operator>> operatorPair_;
};

#endif	/* OPERATORBINARY_H */

