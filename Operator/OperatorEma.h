/* 
 * File:   OperatorEma.h
 * Author: volchnik
 *
 * Created on May 24, 2015, 11:22 PM
 */
#include "OperatorUnary.h"

#ifndef OPERATOREMA_H
#define	OPERATOREMA_H

class OperatorEma : public OperatorUnary {
public:
    OperatorEma(std::shared_ptr<Operator> operator_ptr, double coef);
    OperatorEma(const OperatorEma& orig);
    virtual ~OperatorEma();
    
    OperatorEma* Clone() const;
    
    std::shared_ptr<Operator> perform() const;
    std::string ToString() const;
private:

};

#endif	/* OPERATOREMA_H */

