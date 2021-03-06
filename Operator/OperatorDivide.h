/* 
 * File:   OperatorDivide.h
 * Author: volchnik
 *
 * Created on June 6, 2015, 10:12 AM
 */

#include "OperatorBinary.h"

#ifndef OPERATORDIVIDE_H
#define	OPERATORDIVIDE_H

class OperatorDivide : public OperatorBinary {
public:
    OperatorDivide(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second);
    OperatorDivide(const OperatorDivide& orig);
    virtual ~OperatorDivide();
    
    OperatorDivide* Clone() const;
    
    std::shared_ptr<Operator> perform() const;
    std::string ToString() const;
private:

};

#endif	/* OPERATORDIVIDE_H */

