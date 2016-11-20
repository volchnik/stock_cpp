//
//  OperatorAtan.h
//  stock_cpp
//
//  Created by NIKITA VOLCHEGORSKIY on 28/06/16.
//  Copyright © 2016 NIKITA VOLCHEGORSKIY. All rights reserved.
//
#include "OperatorUnary.h"

#ifndef OperatorAtan_h
#define OperatorAtan_h

class OperatorAtan : public OperatorUnary {
public:
    OperatorAtan(std::shared_ptr<Operator> operator_ptr);
    OperatorAtan(const OperatorAtan& orig);
    virtual ~OperatorAtan();
    
    OperatorAtan* Clone() const;
    
    std::shared_ptr<Operator> perform() const;
    std::string ToString() const;
private:
    
};

#endif /* OperatorAtan_h */
