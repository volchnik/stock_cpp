/* 
 * File:   OperatorAdd.h
 * Author: volchnik
 *
 * Created on April 25, 2015, 11:07 AM
 */

#include "OperatorBinary.h"

#ifndef OPERATORADD_H
#define	OPERATORADD_H

class OperatorAdd : public OperatorBinary {
public:
    OperatorAdd(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second);
    OperatorAdd(const OperatorAdd& orig);
    virtual ~OperatorAdd();
    
    OperatorAdd* Clone();
    
    std::shared_ptr<Operator> perform();
    std::string ToString();
private:

};

#endif	/* OPERATORADD_H */

