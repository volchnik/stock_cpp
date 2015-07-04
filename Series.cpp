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
    this->series_ = orig.series_;
    this->datetime_map_ = orig.datetime_map_;
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

        DayOfTheYear day_of_year(gmtime(&datetime_sample));
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

    for (auto& sample : this->series_) {
        if (sample.datetime != datetime_sample) {

            sample_insert.volume = floor(sample_insert.volume / (double) sample_count);
            
            if (gmtime(&sample.datetime)->tm_yday != gmtime(&datetime_sample)->tm_yday) {
                sample_insert.datetime = datetime_sample + 1;
                normalized_series.push_back(sample_insert);
                InsertIntoDateTimeMap(&datetime_map_, datetime_sample, series_index_start, normalized_series.size());
                series_index_start = normalized_series.size();
            } else {
                for (long time_delta = 0; time_delta < sample.datetime - datetime_sample; time_delta++) {

                    sample_insert.datetime = datetime_sample + time_delta + 1;
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

double Series::GetValue(const long datetime) const {
    DayOfTheYear day_of_year(gmtime(&datetime));
    map<DayOfTheYear, SeriesInterval>::const_iterator findIterator = datetime_map_.find(day_of_year);
    if (findIterator == datetime_map_.end()) {
        throw new out_of_range("Value request time is out of range for current series");
    }
    SeriesInterval series_interval = findIterator->second;

    if (datetime >= this->series_.at(series_interval.begin_interval).datetime
            && datetime <= this->series_.at(series_interval.end_interval - 1).datetime) {
        return this->series_.at(series_interval.begin_interval +
                datetime - this->series_.at(series_interval.begin_interval).datetime).value;
    } else if (findIterator != datetime_map_.begin()) {
        return this->series_.at((--findIterator)->second.end_interval - 1).value;
    } else {
        throw new out_of_range("Value request time is out of range for current series");
    }
}

void Series::SetValue(const long datetime, const double& value) {
    DayOfTheYear day_of_year(gmtime(&datetime));
    map<DayOfTheYear, SeriesInterval>::const_iterator findIterator = datetime_map_.find(day_of_year);
    if (findIterator == datetime_map_.end()) {
        throw new out_of_range("Value request time is out of range for current series");
    }
    SeriesInterval series_interval = findIterator->second;

    if (datetime >= this->series_.at(series_interval.begin_interval).datetime
            && datetime <= this->series_.at(series_interval.end_interval - 1).datetime) {
        this->series_.at(series_interval.begin_interval +
                datetime - this->series_.at(series_interval.begin_interval).datetime).value = value;
    } else {
        throw new out_of_range("Value request time is out of range for current series");
    }
}

Series& Series::operator=(const Series& series) {
    if (this != &series) {
        this->series_ = series.series_;
        this->datetime_map_ = series.datetime_map_;
    }
    return *this;
}

Series& Series::operator+=(const Series& series) {
    for (auto& sample : this->series_) {
        sample.value += series.GetValue(sample.datetime);
    }
    return *this;
}

const Series Series::operator+(const Series& series) const{
    return Series(*this) += series;
}

Series& Series::operator-=(const Series& series) {
    for (auto& sample : this->series_) {
        sample.value -= series.GetValue(sample.datetime);
    }
    return *this;
}

const Series Series::operator-(const Series& series) const{
    return Series(*this) -= series;
}

Series& Series::operator*=(const double& multiplier) {
    for (auto& sample : this->series_) {
        sample.value *= multiplier;
    }
    return *this;
}

Series& Series::operator/=(const double& divider) {
    if (divider == 0) {
        throw std::overflow_error("Divide by zero exception");
    }
    return (*this) *= (1 / divider);
}

Series& Series::operator/=(const Series& series) {
    double divider = 0.0;
    for (auto& sample : this->series_) {
        divider = series.GetValue(sample.datetime);
        if (divider != 0.0) {
            sample.value /= divider;
        } else {
            sample.value = 0.0;
        }
    }
    return *this;
}

Series& Series::operator*=(const Series& series) {
    for (auto& sample : this->series_) {
        sample.value *= series.GetValue(sample.datetime);
    }
    return *this;
}

const Series Series::operator*(const double& multiplier) const {
    return Series(*this) *= multiplier;
}

const Series Series::operator*(const Series& series) const {
    return Series(*this) *= series;
}

const Series operator*(const double& multiplier, const Series& series){
    return series * multiplier;
}

const Series operator/(const double& dividend, const Series& series){
    Series returnSeries(series);
    for (auto& sample : returnSeries.series_) {
        sample.value = dividend / sample.value;
    }
    
    return returnSeries;
}

const Series Series::operator/(const double& divider) const {
    return Series(*this) /= divider;
}

const Series Series::operator/(const Series& series) const {
    return Series(*this) /= series;
}

Series& Series::operator+=(const double& level) {
    for (auto& sample : this->series_) {
        sample.value += level;
    }
    return *this;
}

const Series Series::operator+(const double& level) const {
    return Series(*this) += level;
}

Series& Series::operator-=(const double& level) {
    for (auto& sample : this->series_) {
        sample.value -= level;
    }
    return *this;
}

const Series Series::operator-(const double& level) const {
    return Series(*this) -= level;
}

const Series operator+(const double& level, const Series& series) {
    return series + level;
}

const Series operator-(const double& level, const Series& series) {
    return series - level;
}

const Series Series::EmaIndicator(long delta) const {    
    if(delta <= 1 || this->series_.size() <= 2) {
        return *this;
    }
    
    Series copy(*this);
    
    long lastIntervalValueCurrent = 0;
    long lastIntervalValueOld = copy.series_.begin()->value;

    for (auto sample = copy.series_.begin() + 1; (sample + 1) != copy.series_.end(); sample++) {
        
        if ((sample + 1)->datetime > sample->datetime + 1) {
            lastIntervalValueCurrent = sample->value;
        }
        if (sample->datetime == (sample - 1)->datetime + 1) {
            sample->value = ((sample - 1)->value * (delta - 1) + sample->value * 2 ) / (double)(delta + 1);
        } else {
            double tempValue = (sample - 1)->value;
            for (long datetimeIndex = (sample - 1)->datetime + 1; datetimeIndex < sample->datetime; datetimeIndex++) {
                tempValue = (tempValue * (delta - 1) + lastIntervalValueOld * 2 ) / (double)(delta + 1);
            }
            sample->value = (tempValue * (delta - 1) + sample->value * 2 ) / (double)(delta + 1);
        }
        
        lastIntervalValueOld = lastIntervalValueCurrent;
    }
    return copy;
}

const Series Series::SmaIndicator(long delta) const {
    if(delta <= 1 || this->series_.size() <= delta) {
        return *this;
    }
    
    Series copy(*this);
    Series copyResult(*this);
    bool gotInitial = false;
    
    auto sampleCopy = copy.series_.begin();
    
    for (auto sample = copyResult.series_.begin(); 
            sample != copyResult.series_.end(), sampleCopy != copy.series_.end(); 
            sample++, sampleCopy++) {
        if (sample->datetime - copyResult.series_.begin()->datetime <  delta) {
            sample->value *= delta;
        } else if ((sample->datetime - (sample - delta)->datetime) == delta && gotInitial) {
            sample->value = (sample - 1)->value + (sample->value - (sampleCopy - delta)->value);
        } else {
            double initial = 0.0;
            for(long datetime = sample->datetime; datetime > sample->datetime - delta; datetime--) {
                initial += copy.GetValue(datetime);
            }
            sample->value = initial;
            gotInitial = true;
        }
    }
    
    copyResult /= (double)delta;
    
    return copyResult;
}

const Series Series::GenerateTradeAllowSingal(TimeOfDay tradeBegin, TimeOfDay tradeEnd, int cooldownSeconds) const {
    Series allowSeries(*this * 0.0);
    
    for (auto& date : this->datetime_map_) {
        int dayAllowInterval = 0;
        int monthAllowInterval = 0;
        Helpers::GetDayMonthfromDayOfTheYear(date.first.year_corrected, date.first.day_of_year, Helpers::Timezone::msk,
                monthAllowInterval, dayAllowInterval);

        long beginAllowInterval = Helpers::GetTimeUtcFromTimezone(date.first.year_corrected + 1900,
                monthAllowInterval, dayAllowInterval,
                tradeBegin.hour, tradeBegin.minute, tradeBegin.second, Helpers::Timezone::msk);

        long endAllowInterval = Helpers::GetTimeUtcFromTimezone(date.first.year_corrected + 1900,
                monthAllowInterval, dayAllowInterval,
                tradeEnd.hour, tradeEnd.minute, tradeEnd.second, Helpers::Timezone::msk);
        for (long index = date.second.begin_interval; index < date.second.end_interval; index++) {
            if (this->series_.at(index).datetime >= beginAllowInterval && 
                    this->series_.at(index).datetime < endAllowInterval - cooldownSeconds) {
                allowSeries.series_.at(index).value = 1.0;
            } else if (this->series_.at(index).datetime < endAllowInterval) {
                allowSeries.series_.at(index).value = (endAllowInterval - this->series_.at(index).datetime) /
                        (double)cooldownSeconds;
            } else {
                allowSeries.series_.at(index).value = 0.0;
            }
        }
    }
    
    return allowSeries;
}

void Series::PlotGnu(long step, vector<Series> plotSerieses) {
    if (plotSerieses.empty()) {
        return;
    }
    printf("plot");
    
    int index = 1;
    for (auto& series : plotSerieses) {
        printf("'-' using 1:2 with lines,");
    }
    
    printf("\n");
    
    for (auto& series : plotSerieses) {
        for (auto sample = series.series_.begin();
                sample < series.series_.end();
                sample += min(step, series.series_.end() - sample)) {
            printf("%ld \t %.8f \n", sample->datetime, sample->value);
        }
        printf("\ne\n");
    }
    
}
