/* 
 * File:   SeriesInterval.h
 * Author: volchnik
 *
 * Created on September 29, 2014, 1:36 AM
 */

#ifndef SERIESINTERVAL_H
#define	SERIESINTERVAL_H

struct SeriesInterval 
{
    long begin_interval;
    long end_interval;
    
    // Данные датасерии
    vector<SeriesSample> series_;
    
    SeriesInterval(const vector<SeriesSample>& series, long begin_interval, long end_interval) : series_(series), begin_interval(begin_interval), end_interval(end_interval) {
    }
};

#endif	/* SERIESINTERVAL_H */

