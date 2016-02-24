/* 
 * File:   Helpers.cpp
 * Author: volchnik
 * 
 * Created on November 5, 2014, 1:43 AM
 */

#include "Helpers.h"

time_t Helpers::mktimeTimezone(struct tm *src) {
    time_t offset = src->tm_gmtoff;
    return timegm(src) - offset;
}

int Helpers::GetTimezoneOffset(struct tm *src, Timezone timezone) {
    if (timezone != Timezone::msk) {
        return 0;
    }

    src->tm_gmtoff = 0;
    time_t currentTime = mktimeTimezone(src);
    
    struct tm date2014;
    memset(&date2014, 0, sizeof (tm));
    date2014.tm_year = 2014 - 1900;
    date2014.tm_mon = 9;
    date2014.tm_mday = 26;
    date2014.tm_hour = 3;
    date2014.tm_gmtoff = 0;
    time_t date2014Time = mktimeTimezone(&date2014);
    
    
    
    if (currentTime >= date2014Time) {
        return 3;
    } else {
        struct tm date2011;
        memset(&date2011, 0, sizeof (tm));
        date2011.tm_year = 2011 - 1900;
        date2011.tm_mon = 2;
        date2011.tm_mday = 27;
        date2011.tm_hour = 3;
        date2011.tm_gmtoff = 0;
        time_t date2011Time = mktimeTimezone(&date2011);
        
        if (currentTime >= date2011Time) {
            return 4;
        } else {
            struct tm dateCompareMarch;
            memset(&dateCompareMarch, 0, sizeof (tm));
            dateCompareMarch.tm_year = src->tm_year;
            dateCompareMarch.tm_mon = 2;
            dateCompareMarch.tm_mday = 27;
            dateCompareMarch.tm_hour = 3;
            dateCompareMarch.tm_gmtoff = 0;
            time_t dateMarchTime = mktimeTimezone(&dateCompareMarch);

            struct tm dateCompareNovember;
            memset(&dateCompareNovember, 0, sizeof (tm));
            dateCompareNovember.tm_year = src->tm_year;
            dateCompareNovember.tm_mon = 9;
            dateCompareNovember.tm_mday = 26;
            dateCompareNovember.tm_hour = 3;
            dateCompareNovember.tm_gmtoff = 0;
            time_t dateNovemberTime = mktimeTimezone(&dateCompareNovember);

            if (currentTime >= dateMarchTime && currentTime < dateNovemberTime) {
                return 3;
            } else {
                return 4;
            }
        }
    }
}

time_t Helpers::GetTimeUtcFromTimezone(int year, int month, int date, int hour, int munite, int second, Timezone timezone) {
    tm time_structure;
    time_structure.tm_year = year - 1900;
    time_structure.tm_mon = month - 1;
    time_structure.tm_mday = date;
    time_structure.tm_hour = hour;
    time_structure.tm_min = munite;
    time_structure.tm_sec = second;
    time_structure.tm_gmtoff = Helpers::GetTimezoneOffset(&time_structure, timezone) * 60 * 60;
    return Helpers::mktimeTimezone(&time_structure);
}

void Helpers::GetDayMonthfromDayOfTheYear(int yearCorrected, int dayOfTheYear, Timezone timezone, int &month, int &day) {
    tm time_structure;
    memset(&time_structure, 0, sizeof(tm));
    time_structure.tm_year = yearCorrected;
    time_structure.tm_mday = dayOfTheYear + 1;
    Helpers::mktimeTimezone(&time_structure);
    month = time_structure.tm_mon + 1;
    day = time_structure.tm_mday;
}

tm * Helpers::gmtime(register const time_t *timer, struct tm *timep)
{
    time_t time = *timer;
    register unsigned long dayclock, dayno;
    int year = EPOCH_YR;
    
    dayclock = (unsigned long)time % SECS_DAY;
    dayno = (unsigned long)time / SECS_DAY;
    
    timep->tm_sec = dayclock % 60;
    timep->tm_min = (dayclock % 3600) / 60;
    timep->tm_hour = dayclock / 3600;
    timep->tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */
    while (dayno >= YEARSIZE(year)) {
        dayno -= YEARSIZE(year);
        year++;
    }
    timep->tm_year = year - YEAR0;
    timep->tm_yday = dayno;
    timep->tm_mon = 0;
    while (dayno >= _ytab[LEAPYEAR(year)][timep->tm_mon]) {
        dayno -= _ytab[LEAPYEAR(year)][timep->tm_mon];
        timep->tm_mon++;
    }
    timep->tm_mday = dayno + 1;
    timep->tm_isdst = 0;
    
    return timep;
}

string Helpers::exec_bash(const string& cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

