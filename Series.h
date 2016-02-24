/* 
 * File:   Series.h
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:47 PM
 */
#include "root.h"
#include "SeriesSampleExtended.h"
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
    Series(const std::string& series_name);
    Series(const Series& orig);
    virtual ~Series();
    
    static void LoadFromFinamTickFile(vector<SeriesSampleExtended>& series, const string& fileName);
    void Normalize(const vector<SeriesSampleExtended>& series);
    
    std::string GetName() const;
    void SetName(const std::string& series_name);
    float GetValue(const long datetime, std::shared_ptr<DayOfTheYear> pday_of_the_year = nullptr) const;
    void SetValue(const long datetime, const float& value, std::shared_ptr<DayOfTheYear> pday_of_the_year = nullptr);
    
    Series& operator=(const Series& series);
    Series& operator+=(const Series& series);
    const Series operator+(const Series& series) const;
    Series& operator-=(const Series& series);
    const Series operator-(const Series& series) const;
    Series& operator*=(const double& multiplier);
    Series& operator*=(const Series& series);
    Series& operator/=(const double& divider);
    Series& operator/=(const Series& series);
    const Series operator*(const double& multiplier) const;
    const Series operator*(const Series& series) const;
    friend const Series operator*(const double& multiplier, const Series& series);
    const Series operator/(const double& divider) const;
    const Series operator/(const Series& series) const;
    friend const Series operator/(const double& dividend, const Series& series);
    Series& operator+=(const double& level);
    const Series operator+(const double& level) const;
    Series& operator-=(const double& level);
    const Series operator-(const double& level) const;
    friend const Series operator+(const double& level, const Series& series);
    friend const Series operator-(const double& level, const Series& series);
    
    const Series EmaIndicator(long delta) const;
    const Series SmaIndicator(long delta) const;
    
    const Series GenerateTradeAllowSingal(TimeOfDay tradeBegin, TimeOfDay tradeEnd, int cooldownSeconds = 1800) const;
    const Series GenerateZeroBaseSeries() const;
    
    static void PlotGnu(const char* file_name, long step, vector<Series> plotSerieses);
protected:
private:
    // Наименование серии
    std::string series_name_;
    // Справочник отступов индексов времени внутри дней
    map<DayOfTheYear, SeriesInterval> datetime_map_;
};

#endif	/* SERIES_H */

