/* 
 * File:   OperatorMultiply.h
 * Author: volchnik
 *
 * Created on June 6, 2015, 10:10 AM
 */
#include "OperatorBinary.h"

#ifndef OPERATORMULTIPLY_H
#define	OPERATORMULTIPLY_H

class OperatorMultiply : public OperatorBinary {
public:
    OperatorMultiply(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second);
    OperatorMultiply(const OperatorMultiply& orig);
    virtual ~OperatorMultiply();
    
    OperatorMultiply* Clone() const;
    
    std::shared_ptr<Operator> perform() const;
    std::string ToString() const;
private:

};

#endif	/* OPERATORMULTIPLY_H */

