/* 
 * File:   OperatorSeries.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 10:13 PM
 */
#include "Operator.h"
#include "../Series.h"

#ifndef OPERATORSERIES_H
#define	OPERATORSERIES_H

class OperatorSeries : public Operator {
public:
    OperatorSeries(std::shared_ptr<Series> series_ptr);
    OperatorSeries(const OperatorSeries& orig);
    virtual ~OperatorSeries();
    
    std::shared_ptr<Operator> perform() { return make_shared<OperatorSeries>(*this); };
    std::shared_ptr<Series> getSeries();
    std::string ToString();
private:
    std::shared_ptr<Series> series_ptr_;
};

#endif	/* OPERATORSERIES_H */

