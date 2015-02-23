/* 
 * File:   Series.h
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:47 PM
 */
#include "root.h"
#include "SeriesSample.h"
#include "DayOfTheYear.h"
#include "Series/TimeOfDay.h"
#include "SeriesInterval.h"
#include "Helpers.h"

#ifndef SERIES_H
#define	SERIES_H

class Series {
    friend class Trader;
public:
    Series();
    Series(const Series& orig);
    virtual ~Series();
    
    void LoadFromFinamTickFile(const char* fileName);
    void Normalize();
    
    double GetValue(const long datetime) const;
    
    Series& operator=(const Series& series);
    Series& operator+=(const Series& series);
    const Series operator+(const Series& series) const;
    Series& operator-=(const Series& series);
    const Series operator-(const Series& series) const;
    Series& operator*=(const double& multiplier);
    Series& operator/=(const double& divider);
    const Series operator*(const double& multiplier) const;
    friend const Series operator*(const double& multiplier, const Series& series);
    const Series operator/(const double& divider) const;
    
    const Series EmaIndicator(long delta) const;
    const Series SmaIndicator(long delta) const;
    
    const Series GenerateTradeAllowSingal(TimeOfDay tradeBegin, TimeOfDay tradeEnd, int cooldownSeconds = 1800) const;
    
    void PlotGnu(long step) const;
private:
    // Данные датасерии
    vector<SeriesSample> series_;
    // Справочник отступов индексов времени внутри дней
    map<DayOfTheYear, SeriesInterval> datetime_map_;
};

#endif	/* SERIES_H */

