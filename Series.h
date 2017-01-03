/* 
 * File:   Series.h
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:47 PM
 */
#include "root.h"
#include "SeriesSampleExtended.h"
#include "SeriesSample.h"
#include "DayOfTheYear.h"
#include "Series/TimeOfDay.h"
#include "SeriesInterval.h"
#include "Helpers.h"

#ifndef SERIES_H
#define SERIES_H

class Series {
  friend class Trader;
 public:
  Series(const std::string &series_name);
  Series(const Series &orig);
  virtual ~Series();

  static void LoadFromFinamTickFile(vector<SeriesSampleExtended> &series, const string &fileName);
  void Normalize(const vector<SeriesSampleExtended> &series);

  std::string GetName() const;
  void SetName(const std::string &series_name);
  float GetValue(const long datetime, std::shared_ptr<DayOfTheYear> pday_of_the_year = nullptr) const;
  void SetValue(const long datetime, const float &value, std::shared_ptr<DayOfTheYear> pday_of_the_year = nullptr);

  void SetDateTimeMap(map<DayOfTheYear, SeriesInterval> datetime_map);

  Series &operator=(const Series &series);
  Series &operator+=(const Series &series);
  const Series operator+(const Series &series) const;
  Series &operator-=(const Series &series);
  const Series operator-(const Series &series) const;
  Series &operator*=(const double &multiplier);
  Series &operator*=(const Series &series);
  Series &operator/=(const double &divider);
  Series &operator/=(const Series &series);
  const Series operator*(const double &multiplier) const;
  const Series operator*(const Series &series) const;
  friend const Series operator*(const double &multiplier, const Series &series);
  const Series operator/(const double &divider) const;
  const Series operator/(const Series &series) const;
  friend const Series operator/(const double &dividend, const Series &series);
  Series &operator+=(const double &level);
  const Series operator+(const double &level) const;
  Series &operator-=(const double &level);
  const Series operator-(const double &level) const;
  friend const Series operator+(const double &level, const Series &series);
  friend const Series operator-(const double &level, const Series &series);

  const Series EmaIndicator(long delta) const;
  const Series SmaIndicator(long delta) const;

  const Series AtanIndicator() const;
  const Series LogIndicator() const;

  const Series GenerateTradeAllowSingal(TimeOfDay tradeBegin, TimeOfDay tradeEnd, int cooldownSeconds = 1800) const;
  const Series GenerateZeroBaseSeries() const;

  enum ChartsFormat { gnuplot, google, plotly };

  static void GenerateCharts(string file_name,
                             ChartsFormat format,
                             long step,
                             vector<Series> plotSerieses,
                             ulong day_offset = 0);
  static void GenerateGroupedCharts(string file_name,
                                    ChartsFormat format,
                                    long step,
                                    vector<vector<Series>> plotGroupSerieses,
                                    ulong day_offset = 0);

  shared_ptr<Series> getSubSeries(ulong offset, ulong interval);
 protected:
 private:
  // Наименование серии
  std::string series_name_;
  // Справочник отступов индексов времени внутри дней
  map<DayOfTheYear, SeriesInterval> datetime_map_;
};

typedef map<string, Series> series_map_type;
typedef map<string, shared_ptr<Series>> series_ptr_map_type;

#endif	/* SERIES_H */

