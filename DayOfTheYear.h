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
    int year_corrected;
    int day_of_year;

    DayOfTheYear(tm* datetime) : year_corrected(datetime->tm_year), day_of_year(datetime->tm_yday) {
    }

    bool operator<(const DayOfTheYear& day_of_year) const {
        return (this->year_corrected * 365 + this->day_of_year < day_of_year.year_corrected * 365 + day_of_year.day_of_year);
    }

    bool operator==(const DayOfTheYear& day_of_year) const {
        return this->year_corrected == day_of_year.year_corrected && this->day_of_year == day_of_year.day_of_year;
    }
};


#endif	/* DAYOFTHEYEAR_H */

