/* 
 * File:   OperatorSeries.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 10:13 PM
 */

#include "OperatorSeries.h"

OperatorSeries::OperatorSeries(std::shared_ptr<Series> series_ptr) {
    this->series_ptr_ = series_ptr;
}

OperatorSeries::OperatorSeries(const OperatorSeries& orig) {
    this->series_ptr_ = orig.series_ptr_;
}

OperatorSeries::~OperatorSeries() {
}

std::shared_ptr<Series> OperatorSeries::getSeries() {
    return this->series_ptr_;
}

