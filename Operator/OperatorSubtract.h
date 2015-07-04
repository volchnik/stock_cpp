/* 
 * File:   OperatorSubtract.h
 * Author: volchnik
 *
 * Created on June 6, 2015, 9:31 AM
 */
#include "OperatorBinary.h"

#ifndef OPERATORSUBTRACT_H
#define	OPERATORSUBTRACT_H

class OperatorSubtract : public OperatorBinary {
public:
    OperatorSubtract(std::shared_ptr<Operator> first, std::shared_ptr<Operator> second);
    OperatorSubtract(const OperatorSubtract& orig);
    virtual ~OperatorSubtract();
    
    std::shared_ptr<Operator> perform();
private:

};

#endif	/* OPERATORSUBTRACT_H */

