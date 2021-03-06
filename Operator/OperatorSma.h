/* 
 * File:   OperatorSma.h
 * Author: volchnik
 *
 * Created on June 6, 2015, 1:18 AM
 */
#include "OperatorUnary.h"

#ifndef OPERATORSMA_H
#define	OPERATORSMA_H

class OperatorSma : public OperatorUnary {
public:
    OperatorSma(std::shared_ptr<Operator> operator_ptr, double coef);
    OperatorSma(const OperatorSma& orig);
    virtual ~OperatorSma();
    
    OperatorSma* Clone() const;
    
    std::shared_ptr<Operator> perform() const;
    std::string ToString() const;
private:

};

#endif	/* OPERATORSMA_H */

