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

/**
 * Loads data series file from "finam" formatted source file
 * @param fileName name of "finam" data file
 */
void Series::LoadFromFinamTickFile(const char* fileName) {
    fstream file_series;
    file_series.open(fileName, ios::in);

    string line;
    SeriesSample insert_sample;
    getline(file_series, line);

    while (getline(file_series, line)) {
        vector<string> splitLine = split(line, ';');
        
        insert_sample.datetime = 
                Helpers::GetTimeUtcFromTimezone(atoi(splitLine[2].substr(0, 4).c_str()), 
                atoi(splitLine[2].substr(4, 2).c_str()), 
                atoi(splitLine[2].substr(6, 2).c_str()), 
                atoi(splitLine[3].substr(0, 2).c_str()), 
                atoi(splitLine[3].substr(2, 2).c_str()), 
                atoi(splitLine[3].substr(4, 2).c_str()), Helpers::Timezone::msk);
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

/**
 * Normalizes data series by seconds
 */
void Series::Normalize() {
    vector <SeriesSample> normalized_series;
    SeriesSample sample_insert;
    int sample_count = 0;
    long datetime_sample = this->series_.begin()->datetime;
    sample_insert.volume = 0;
    long series_index_start = 0;

    for (auto sample : this->series_) {
        if (sample.datetime != datetime_sample) {

            sample_insert.volume = floor(sample_insert.volume / (double) sample_count);
            
            if (localtime(&sample.datetime)->tm_yday != localtime(&datetime_sample)->tm_yday) {
                normalized_series.push_back(sample_insert);
                InsertIntoDateTimeMap(&datetime_map_, datetime_sample, series_index_start, normalized_series.size());
                series_index_start = normalized_series.size() + 1;
            } else {
                for (long time_delta = 0; time_delta < sample.datetime - datetime_sample; time_delta++) {

                    sample_insert.datetime = datetime_sample + time_delta;
                    if (time_delta > 0) {
                        sample_insert.volume = 0;
                    }
                    normalized_series.push_back(sample_insert);
                }
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
    map<DayOfTheYear, SeriesInterval>::iterator findIterator = datetime_map_.find(day_of_year);
    if (findIterator == datetime_map_.end()) {
        throw new out_of_range("Value request time is out of range for current series");
    }
    SeriesInterval series_interval = findIterator->second;

    if(datetime > this->series_.at(series_interval.begin_interval).datetime
            && datetime < this->series_.at(series_interval.end_interval).datetime) {
        return this->series_.at(series_interval.begin_interval + 
                datetime - this->series_.at(series_interval.begin_interval).datetime).value;
    }
    return 0;
}

