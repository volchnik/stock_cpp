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

    Series series;
    
    series.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140814_140814.txt");
    series.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140815_140815.txt");
    series.Normalize();

    Series seriesEma100 = series.EmaIndicator(100);
    Series seriesEma1000 = series.EmaIndicator(1000);
    Series seriesDiff = 0.01 * (seriesEma100 - seriesEma1000);
    
    TimeOfDay beginTod = {10, 0, 1};
    TimeOfDay endTod = {17, 0, 0};
    Series allowSeries = series.GenerateTradeAllowSingal(beginTod, endTod, 3600);

    Trader trader(series, seriesDiff, allowSeries);
    
    cout << trader.Trade() << endl;

    return 0;
}

