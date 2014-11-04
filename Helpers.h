/* 
 * File:   Helpers.h
 * Author: volchnik
 *
 * Created on November 5, 2014, 1:43 AM
 */
#include "root.h"
#ifndef HELPERS_H
#define	HELPERS_H

class Helpers {
public:
    enum Timezone {msk};
    static time_t mktimeTimezone(struct tm *src);
    static int GetTimezoneOffset(struct tm *src, Timezone timezone);
    static time_t GetTimeUtcFromTimezone(int year, int month, int date, int hour, int munite, int second, Timezone timezone);
private:

};

#endif	/* HELPERS_H */

