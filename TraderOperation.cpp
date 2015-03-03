/* 
 * File:   TraderOperation.cpp
 * Author: volchnik
 * 
 * Created on March 1, 2015, 6:58 PM
 */

#include "TraderOperation.h"

TraderOperation::TraderOperation() : duration_(0), operationType_(operationType::STAY) {
}

TraderOperation::TraderOperation(long duration, operationType operationType) :
duration_(duration), operationType_(operationType) {   
}

TraderOperation::~TraderOperation() {
}

long TraderOperation::GetDuration() {
    return this->duration_;
}

TraderOperation::operationType TraderOperation::GetOperationType() {
    return this->operationType_;
}

void TraderOperation::SetDuration(long duration) {
    this->duration_ = duration;
}
void TraderOperation::SetOperationType(operationType operationType) {
    this->operationType_ = operationType;
}

