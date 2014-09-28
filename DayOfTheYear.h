/* 
 * File:   DayOfTheYear.h
 * Author: volchnik
 *
 * Created on September 29, 2014, 1:11 AM
 */

#ifndef DAYOFTHEYEAR_H
#define	DAYOFTHEYEAR_H
struct DayOfTheYear
{
    int year;
    int day_of_year;
    
    DayOfTheYear(tm* datetime) : year(datetime->tm_year), day_of_year(datetime->tm_yday) {
    }
    
    bool operator < (const DayOfTheYear& day_of_year) const
    {
        return (this->year * 365 + this->day_of_year < day_of_year.year*365 + day_of_year.day_of_year);
    }
    
    long GetDatetimeOffset(long datetime_seconds) {
        tm datetime;
        memset(&datetime, sizeof(tm), 0);
        datetime.tm_year = this->year;
        datetime.tm_yday = this->day_of_year;
        return datetime_seconds - mktime(&datetime);
    }
};


#endif	/* DAYOFTHEYEAR_H */

