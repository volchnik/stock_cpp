/* 
 * File:   Operator.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 9:23 PM
 */

#include "../root.h"
#include <memory>

#ifndef OPERATOR_H
#define	OPERATOR_H

class Operator {
public:
    Operator();
    Operator(const Operator& orig);
    virtual ~Operator();
    
    virtual Operator* Clone() const = 0;
    
    virtual std::shared_ptr<Operator> perform() const = 0;
    
    virtual std::string ToString() const = 0;
private:

};

#endif	/* OPERATOR_H */

