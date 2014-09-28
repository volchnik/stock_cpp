/* 
 * File:   Series.cpp
 * Author: volchnik
 * 
 * Created on August 3, 2014, 10:47 PM
 */
#include "Series.h"

Series::Series() {
}

Series::Series(const Series& orig) {
}

Series::~Series() {
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void Series::LoadFromFinamTickFile(const char* fileName) {
    fstream file_series;
    file_series.open(fileName, ios::in);

    string line;
    SeriesSample insert_sample;
    tm time_structure;
    memset(&time_structure, 0, sizeof(tm));
    getline(file_series, line);

    while (getline(file_series, line)) {
        vector<string> splitLine = split(line, ';');
        
        time_structure.tm_year = atoi(splitLine[2].substr(0, 4).c_str());
        time_structure.tm_mon = atoi(splitLine[2].substr(4, 2).c_str());
        time_structure.tm_mday = atoi(splitLine[2].substr(6, 2).c_str());
        time_structure.tm_hour = atoi(splitLine[3].substr(0, 2).c_str());
        time_structure.tm_min = atoi(splitLine[3].substr(2, 2).c_str());
        time_structure.tm_sec = atoi(splitLine[3].substr(4, 2).c_str());
        insert_sample.datetime = mktime(&time_structure);
        insert_sample.value = atof(splitLine[4].c_str());
        insert_sample.volume = atof(splitLine[5].c_str());
        
        series_.push_back(insert_sample);
    }

    file_series.close();
}

void InsertIntoDateTimeMap(map<DayOfTheYear, SeriesInterval> *map_insert, 
        const long& datetime_sample, const long& interval_start, const long& interval_end) {

        DayOfTheYear day_of_year(localtime(&datetime_sample));
        SeriesInterval series_interval(interval_start, interval_end);
        map_insert->insert(pair<DayOfTheYear, SeriesInterval>(day_of_year, series_interval));
}

void Series::NormalizeSeconds() {
    std::sort(this->series_.begin(), this->series_.end());
    
    vector <SeriesSample> normalized_series;
    SeriesSample sample_insert;
    int sample_count = 0;
    long datetime_sample = this->series_.begin()->datetime;
    sample_insert.volume = 0;
    long series_index_start = 0;

    for (auto sample : this->series_) {
        if (sample.datetime != datetime_sample) {
            sample_insert.volume = floor(sample_insert.volume / (double) sample_count);
            for (long time_delta = 0; time_delta < sample.datetime - datetime_sample; time_delta++) {
                
                if (time_delta > 0 && localtime(&sample.datetime)->tm_yday != localtime(&datetime_sample)->tm_yday) {
                    InsertIntoDateTimeMap(&datetime_map_, datetime_sample, series_index_start, normalized_series.size());
                    series_index_start = normalized_series.size() + 1;
                    break;
                }
                sample_insert.datetime = datetime_sample + time_delta;
                if (time_delta > 0) {
                    sample_insert.volume = 0;
                }
                normalized_series.push_back(sample_insert);
            }

            sample_count = 0;
            datetime_sample = sample.datetime;
            sample_insert.volume = 0;
        }
        sample_insert.value = sample.value;
        sample_insert.volume += sample.volume;
        sample_count++;
    }
    
    InsertIntoDateTimeMap(&datetime_map_, datetime_sample, series_index_start, normalized_series.size());
    
    this->series_ = normalized_series;    
}

double Series::GetValue(long datetime) {
    DayOfTheYear day_of_year(localtime(&datetime));
    SeriesInterval series_interval = datetime_map_.find(day_of_year)->second;

    if(datetime > this->series_.at(series_interval.begin_interval).datetime
            && datetime < this->series_.at(series_interval.end_interval).datetime) {
        return this->series_.at(series_interval.begin_interval + 
                day_of_year.GetDatetimeOffset(datetime) - this->series_.at(series_interval.begin_interval).datetime).value;
    }
    return 0;
}

