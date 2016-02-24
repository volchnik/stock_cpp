/* 
 * File:   Helpers.h
 * Author: volchnik
 *
 * Created on November 5, 2014, 1:43 AM
 */
#include "root.h"
#ifndef HELPERS_H
#define	HELPERS_H

#define YEAR0           1900                    /* the first year */
#define EPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SECS_DAY        (24L * 60L * 60L)
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define TIME_MAX        ULONG_MAX
#define ABB_LEN         3

const int _ytab[2][12] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

class Helpers {
public:
    enum Timezone {msk};
    static time_t mktimeTimezone(struct tm *src);
    static int GetTimezoneOffset(struct tm *src, Timezone timezone);
    static time_t GetTimeUtcFromTimezone(int year, int month, int date, int hour, int munite, int second, Timezone timezone);
    static void GetDayMonthfromDayOfTheYear(int yearCorrected, int dayOfTheYear, Timezone timezone, int &month, int &day);
    static struct tm * gmtime(register const time_t *timer, struct tm *timep);
    static string exec_bash(const string& cmd);
private:

};

#endif	/* HELPERS_H */

