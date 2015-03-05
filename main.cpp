/* 
 * File:   main.cpp
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:24 PM
 */

#include <cstdlib>
#include "Series.h"
#include "Trader.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    srand(time(NULL));
    Series series;
    
    series.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140814_140814.txt");
    series.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140815_140815.txt");
    series.Normalize();

    Series seriesEma80 = series.EmaIndicator(80);
    Series seriesEma100 = series.EmaIndicator(100);
    Series seriesEma1000 = series.EmaIndicator(1000);
    Series seriesDiff = -0.01 * (seriesEma80 - 0.15 * seriesEma100 - 0.85 * seriesEma1000);
    
    TimeOfDay beginTod = {10, 0, 1};
    TimeOfDay endTod = {19, 0, 0};
    Series allowSeries = series.GenerateTradeAllowSingal(beginTod, endTod, 3600);

    Trader trader(series, seriesDiff, allowSeries);
    
    trader.Trade();
    
    vector<Series> plotSeries = {series, allowSeries * 1000.0 + 120000.0, seriesDiff * 1000 + 120000.0, trader.GetTradePosition() * 1000 + 120000.0,
        trader.GetTradeAccount() + 120000.0};
    Series::PlotGnu(10, plotSeries);

    return 0;
}

