//
//  OperatorLog.h
//  stock_cpp
//
//  Created by NIKITA VOLCHEGORSKIY on 16/11/16.
//  Copyright © 2016 NIKITA VOLCHEGORSKIY. All rights reserved.
//
#include "OperatorUnary.h"

#ifndef OperatorLog_h
#define OperatorLog_h

class OperatorLog : public OperatorUnary {
public:
    OperatorLog(std::shared_ptr<Operator> operator_ptr);
    OperatorLog(const OperatorLog& orig);
    virtual ~OperatorLog();
    
    OperatorLog* Clone() const;
    
    std::shared_ptr<Operator> perform() const;
    std::string ToString() const;
private:
    
};

#endif /* OperatorLog_h */
