/* 
 * File:   Series.cpp
 * Author: volchnik
 * 
 * Created on August 3, 2014, 10:47 PM
 */
#include "Series.h"

Series::Series(const std::string& series_name) : series_name_(series_name) {
    if (series_name_.empty()) {
        throw std::invalid_argument("series_name can't be empty");
    }
}

Series::Series(const Series& orig) : datetime_map_(orig.datetime_map_),
    series_name_(orig.series_name_) {
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

std::string Series::GetName() const {
    return this->series_name_;
}

void Series::SetName(const std::string& series_name) {
    this->series_name_ = series_name;
}

/**
 * Loads data series file from "finam" formatted source file
 * @param fileName name of "finam" data file
 */
 void Series::LoadFromFinamTickFile(vector<SeriesSampleExtended>& series, const string& fileName) {
    fstream file_series;
    file_series.open(fileName.c_str(), ios::in);

    string line;
    SeriesSampleExtended insert_sample;

    while (getline(file_series, line)) {
        vector<string> splitLine = split(line, ';');
        
        insert_sample.datetime = 
                Helpers::GetTimeUtcFromTimezone(atoi(splitLine[0].substr(0, 4).c_str()),
                atoi(splitLine[0].substr(4, 2).c_str()),
                atoi(splitLine[0].substr(6, 2).c_str()),
                atoi(splitLine[1].substr(0, 2).c_str()),
                atoi(splitLine[1].substr(2, 2).c_str()),
                atoi(splitLine[1].substr(4, 2).c_str()), Helpers::Timezone::msk);
        insert_sample.value = atof(splitLine[2].c_str());
        
        series.push_back(insert_sample);
    }

    file_series.close();
}

void InsertIntoDateTimeMap(map<DayOfTheYear, SeriesInterval> *map_insert,
                           const long& datetime_sample, vector<SeriesSample>& series, const long& interval_start, const long& interval_end) {
    
    struct tm time_struct;
    DayOfTheYear day_of_year(Helpers::gmtime(&datetime_sample, &time_struct));
    SeriesInterval series_interval(series, interval_start, interval_end);
    map_insert->insert(pair<DayOfTheYear, SeriesInterval>(day_of_year, series_interval));
    series.clear();
}

/**
 * Normalizes data series by seconds
 */
void Series::Normalize(const vector<SeriesSampleExtended>& series) {
    vector <SeriesSample> normalized_series;
    SeriesSample sample_insert;
    long datetime_sample = series.begin()->datetime;
    long series_time_start = series.at(0).datetime;

    struct tm time_struct_1;
    struct tm time_struct_2;
    
    for (auto& sample : series) {
        if (Helpers::gmtime(&sample.datetime, &time_struct_1)->tm_yday != Helpers::gmtime(&datetime_sample, &time_struct_2)->tm_yday) {
            normalized_series.push_back(sample_insert);
            InsertIntoDateTimeMap(&datetime_map_, datetime_sample, normalized_series, series_time_start, series_time_start + normalized_series.size() - 1);
            sample_insert.value = sample.value;
            normalized_series.push_back(sample_insert);
            series_time_start = sample.datetime;
        } else {
            for (long time_delta = 0; time_delta < sample.datetime - datetime_sample; time_delta++) {
                normalized_series.push_back(sample_insert);
            }
        }
        
        datetime_sample = sample.datetime;
        sample_insert.value = sample.value;
    }
    
    InsertIntoDateTimeMap(&datetime_map_, datetime_sample, normalized_series, series_time_start, series_time_start + normalized_series.size() - 1);
}

float Series::GetValue(const long datetime, std::shared_ptr<DayOfTheYear> pday_of_the_year) const {
    struct tm time_struct;
    std::shared_ptr<DayOfTheYear> pday_of_year_local = nullptr;
    if (pday_of_the_year != nullptr) {
        pday_of_year_local = pday_of_the_year;
    } else {
        pday_of_year_local = std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime, &time_struct)));
    }

    map<DayOfTheYear, SeriesInterval>::const_iterator findIterator = datetime_map_.find(*pday_of_year_local);
    if (findIterator == datetime_map_.end()) {
        DayOfTheYear day_of_year_begin(Helpers::gmtime(&(datetime_map_.begin())->second.begin_interval, &time_struct));
        if (day_of_year_begin < *pday_of_year_local) {
            for (auto& day : boost::adaptors::reverse(datetime_map_)) {
                DayOfTheYear day_of_year_current(Helpers::gmtime(&(day.second.begin_interval), &time_struct));
                if (day_of_year_current < *pday_of_year_local) {
                    findIterator = datetime_map_.find(day_of_year_current);
                    break;
                }
            }
        }
        if (findIterator == datetime_map_.end()) {
            throw std::out_of_range("Value request time is out of range for current series");
        }
    }

    if (datetime >= findIterator->second.begin_interval
            && datetime <= findIterator->second.end_interval) {
        return findIterator->second.series_.at(datetime - findIterator->second.begin_interval).value;
    } else if (datetime > findIterator->second.end_interval) {
        return findIterator->second.series_.at(findIterator->second.series_.size() - 1).value;
    } else if (findIterator != datetime_map_.begin()) {
        findIterator--;
        return (findIterator)->second.series_.at((findIterator)->second.series_.size() - 1).value;
    } else {
        throw std::out_of_range("Value request time is out of range for current series");
    }
}

void Series::SetValue(const long datetime, const float& value, std::shared_ptr<DayOfTheYear> pday_of_the_year) {
    struct tm time_struct;
    std::shared_ptr<DayOfTheYear> pday_of_year_local = nullptr;
    if (pday_of_the_year != nullptr) {
        pday_of_year_local = pday_of_the_year;
    } else {
        pday_of_year_local = std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime, &time_struct)));
    }
    
    map<DayOfTheYear, SeriesInterval>::iterator findIterator = datetime_map_.find(*pday_of_year_local);
    if (findIterator == datetime_map_.end()) {
        throw std::out_of_range("Value request time is out of range for current series");
    }

    if (datetime >= findIterator->second.begin_interval
            && datetime <= findIterator->second.end_interval) {
        findIterator->second.series_.at(datetime - findIterator->second.begin_interval).value = value;
    } else {
        throw std::out_of_range("Value request time is out of range for current series");
    }
}

Series& Series::operator=(const Series& series) {
    if (this != &series) {
        this->datetime_map_ = series.datetime_map_;
        this->series_name_ = series.series_name_;
    }
    return *this;
}

Series& Series::operator+=(const Series& series) {
    struct tm time_struct;
    for (auto& datetime_series : this->datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year_local =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        for (auto& sample : datetime_series.second.series_) {
            sample.value += series.GetValue(datetime_series.second.begin_interval + offset, pday_of_year_local);
            offset++;
        }
    }
    this->series_name_ = "(" + this->series_name_+ " + " + series.GetName() + ")";
    return *this;
}

const Series Series::operator+(const Series& series) const{
    return Series(*this) += series;
}

Series& Series::operator-=(const Series& series) {
    struct tm time_struct;
    for (auto& datetime_series : this->datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year_local =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        for (auto& sample : datetime_series.second.series_) {
            sample.value -= series.GetValue(datetime_series.second.begin_interval + offset, pday_of_year_local);
            offset++;
        }
    }
    this->series_name_ = "(" + this->series_name_+ " - " + series.GetName() + ")";
    return *this;
}

const Series Series::operator-(const Series& series) const{
    return Series(*this) -= series;
}

Series& Series::operator*=(const double& multiplier) {
    for (auto& datetime_series : this->datetime_map_) {
        for (auto& sample : datetime_series.second.series_) {
            sample.value *= multiplier;
        }
    }
    this->series_name_ = "(" + this->series_name_+ " * " + std::to_string(multiplier) + ")";
    return *this;
}

Series& Series::operator/=(const double& divider) {
    if (divider == 0) {
        throw std::overflow_error("Divide by zero exception");
    }
    return (*this) *= (1 / divider);
}

Series& Series::operator/=(const Series& series) {
    struct tm time_struct;
    double divider = 0.0;
    for (auto& datetime_series : this->datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year_local =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        for (auto& sample : datetime_series.second.series_) {
            divider = series.GetValue(datetime_series.second.begin_interval + offset, pday_of_year_local);
            if (divider != 0.0) {
                sample.value /= divider;
            } else {
                sample.value = 0.0;
            }
            offset++;
        }
    }

    return *this;
}

Series& Series::operator*=(const Series& series) {
    struct tm time_struct;
    for (auto& datetime_series : this->datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year_local =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        for (auto& sample : datetime_series.second.series_) {
            sample.value *= series.GetValue(datetime_series.second.begin_interval + offset, pday_of_year_local);
            offset++;
        }
    }
    this->series_name_ = "(" + this->series_name_+ " * " + series.GetName() + ")";
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
    for (auto& datetime_series : returnSeries.datetime_map_) {
        for (auto& sample : datetime_series.second.series_) {
            sample.value = dividend / sample.value;
        }
    }
    returnSeries.SetName("(" + std::to_string(dividend) + " / " + series.GetName() + ")");
    return returnSeries;
}

const Series Series::operator/(const double& divider) const {
    return Series(*this) /= divider;
}

const Series Series::operator/(const Series& series) const {
    return Series(*this) /= series;
}

Series& Series::operator+=(const double& level) {
    for (auto& datetime_series : this->datetime_map_) {
        for (auto& sample : datetime_series.second.series_) {
            sample.value += level;
        }
    }
    this->series_name_ = "(" + this->series_name_+ " + " + std::to_string(level) + ")";
    return *this;
}

const Series Series::operator+(const double& level) const {
    return Series(*this) += level;
}

Series& Series::operator-=(const double& level) {
    for (auto& datetime_series : this->datetime_map_) {
        for (auto& sample : datetime_series.second.series_) {
            sample.value -= level;
        }
    }
    this->series_name_ = "(" + this->series_name_+ " - " + std::to_string(level) + ")";
    return *this;
}

const Series Series::operator-(const double& level) const {
    return Series(*this) -= level;
}

const Series operator+(const double& level, const Series& series) {
    return series + level;
}

const Series operator-(const double& level, const Series& series) {
    return -1 * (series - level);
}

const Series Series::EmaIndicator(long delta) const {    
    if(delta <= 1 || this->datetime_map_.size() == 0) {
        return *this;
    }
    
    Series copy(*this);
    
    bool first_pass = true;
    long prev_datetime_end = 0;
    for (auto& datetime_series : copy.datetime_map_) {
        SeriesSample prev_sample;
        SeriesSample prev_sample_preserve;

        if (!first_pass) {
            double tempValue = prev_sample.value;
            for (long datetimeIndex = prev_datetime_end + 1; datetimeIndex < datetime_series.second.begin_interval; datetimeIndex++) {
                tempValue = (tempValue * (delta - 1) + prev_sample.value * 2) / (double)(delta + 1);
            }
            prev_sample.value = tempValue;
        } else {
            prev_sample = *datetime_series.second.series_.begin();
        }
        for (auto& sample : datetime_series.second.series_) {
            prev_sample_preserve = sample;
            sample.value = (prev_sample.value * (delta - 1) + sample.value * 2) / (double)(delta + 1);
            prev_sample = prev_sample_preserve;
        }
        if (datetime_series.second.series_.size() > 0) {
            first_pass = false;
            prev_datetime_end = datetime_series.second.end_interval;
        }
    }

    copy.SetName("EMA(" + copy.GetName() + ", " + std::to_string(delta) + ")");

    return copy;
}

const Series Series::SmaIndicator(long delta) const {
    if(delta <= 1 || this->datetime_map_.size() == 0) {
        return *this;
    }
    
    Series copy(*this);
    struct tm time_struct;
    
    long series_start_time = copy.datetime_map_.begin()->second.begin_interval;
    SeriesSample prev_sample = *copy.datetime_map_.begin()->second.series_.begin();
    for (auto& datetime_series : copy.datetime_map_) {
        unsigned long offset = 0;
        std::shared_ptr<DayOfTheYear> pday_of_year_local =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&datetime_series.second.begin_interval, &time_struct)));
        for (auto& sample : datetime_series.second.series_) {
            if (datetime_series.second.begin_interval + offset - series_start_time < delta) {
                sample.value *= delta;
            } else if (offset < delta) {
                for (long ind_delta = 1; ind_delta < delta; ind_delta++) {
                    sample.value += this->GetValue(datetime_series.second.begin_interval + offset - ind_delta);
                }
            } else {
                sample.value = prev_sample.value + (sample.value - this->GetValue(datetime_series.second.begin_interval + offset - delta, pday_of_year_local));
            }
            prev_sample = sample;
            offset++;
        }
    }
    
    copy /= (double)delta;
    copy.SetName("SMA(" + copy.GetName() + ", " + std::to_string(delta) + ")");

    return copy;
}

const Series Series::GenerateTradeAllowSingal(TimeOfDay tradeBegin, TimeOfDay tradeEnd, int cooldownSeconds) const {
    struct tm time_struct;
    Series allowSeries(*this * 0.0);
    
    for (auto& date : this->datetime_map_) {
        int dayAllowInterval = 0;
        int monthAllowInterval = 0;
        Helpers::GetDayMonthfromDayOfTheYear(date.first.year_corrected, date.first.day_of_year, Helpers::Timezone::msk,
                monthAllowInterval, dayAllowInterval);
        
        std::shared_ptr<DayOfTheYear> pday_of_year_local =
            std::make_shared<DayOfTheYear>(DayOfTheYear(Helpers::gmtime(&date.second.begin_interval, &time_struct)));

        long beginAllowInterval = Helpers::GetTimeUtcFromTimezone(date.first.year_corrected + 1900,
                monthAllowInterval, dayAllowInterval,
                tradeBegin.hour, tradeBegin.minute, tradeBegin.second, Helpers::Timezone::msk);

        long endAllowInterval = Helpers::GetTimeUtcFromTimezone(date.first.year_corrected + 1900,
                monthAllowInterval, dayAllowInterval,
                tradeEnd.hour, tradeEnd.minute, tradeEnd.second, Helpers::Timezone::msk);
        for (long index = date.second.begin_interval; index < date.second.end_interval; index++) {
            if (index >= beginAllowInterval && index < endAllowInterval - cooldownSeconds) {
                allowSeries.SetValue(index, 1.0, pday_of_year_local);
            } else if (index < endAllowInterval) {
                allowSeries.SetValue(index, (endAllowInterval - index) / (double)cooldownSeconds, pday_of_year_local);
            } else {
                allowSeries.SetValue(index, 0.0, pday_of_year_local);
            }
        }
    }
    
    return allowSeries;
}

const Series Series::GenerateZeroBaseSeries() const {
    return *this * 0.0;
}

void Series::PlotGnu(const char* file_name, long step, vector<Series> plotSerieses) {
    if (plotSerieses.empty()) {
        return;
    }
    
    ofstream plot_stream(file_name);
    plot_stream << "set term png size 1024,1024; set output \"plot.png\";plot";
    
    int index = 1;
    for (auto& series : plotSerieses) {
        plot_stream << "'-' using 1:2 with lines,";
    }
    
    plot_stream << "\n";

    for (auto& series : plotSerieses) {

        for (auto& datetime_series : series.datetime_map_) {
            long begin_time = datetime_series.second.begin_interval;
            for (auto sample = datetime_series.second.series_.begin();
                 sample < datetime_series.second.series_.end();
                 sample += min(step, datetime_series.second.series_.end() - sample)) {
                char str[128];
                sprintf(str, "%ld \t %.8f \n", begin_time, sample->value);
                plot_stream << str;
                begin_time += step;
            }
        }
        plot_stream << "\ne\n";
    }
    
    plot_stream.close();
    
}

