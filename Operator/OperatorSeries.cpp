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

OperatorSeries* OperatorSeries::Clone() const
{
    return new OperatorSeries(*this);
}

std::shared_ptr<Series> OperatorSeries::getSeries() {
    return this->series_ptr_;
}

std::string OperatorSeries::ToString() const {
    return std::string(series_ptr_->GetName());
}

void OperatorSeries::RefreshSerieses(map<std::string, std::shared_ptr<Series>> collection) {
    try {
        //cout << this->series_ptr_->GetName() << endl;
        this->series_ptr_ = collection.find(this->series_ptr_->GetName())->second;
    } catch(...) {
        cout << "Dint found series: " << this->series_ptr_->GetName() << endl;
        throw std::runtime_error("Dint found series");
    }
}