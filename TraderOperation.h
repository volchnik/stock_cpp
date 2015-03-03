/* 
 * File:   TraderOperation.h
 * Author: volchnik
 *
 * Created on March 1, 2015, 6:58 PM
 */

#ifndef TRADEROPERATION_H
#define	TRADEROPERATION_H

class TraderOperation {
public:
    enum operationType {STAY, BUY, SELL};
    
    TraderOperation();
    TraderOperation(long duration, operationType operationType);
    virtual ~TraderOperation();
    
    long GetDuration();
    operationType GetOperationType();
    void SetDuration(long duration);
    void SetOperationType(operationType operationType);
private:
    long duration_;
    operationType operationType_;
};

#endif	/* TRADEROPERATION_H */

