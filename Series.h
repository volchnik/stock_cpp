/* 
 * File:   Series.h
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:47 PM
 */
#include "root.h"
#include "SeriesSample.h"
#include "DayOfTheYear.h"
#include "SeriesInterval.h"
#include "Helpers.h"

#ifndef SERIES_H
#define	SERIES_H

class Series {
public:
    Series();
    Series(const Series& orig);
    virtual ~Series();
    
    void LoadFromFinamTickFile(const char* fileName);
    void Normalize();
    
    double GetValue(long datetime);
private:
    // Данные датасерии
    vector<SeriesSample> series_;
    // Справочник отступов индексов времени внутри дней
    map<DayOfTheYear, SeriesInterval> datetime_map_;
};

#endif	/* SERIES_H */

