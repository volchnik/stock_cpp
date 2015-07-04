/* 
 * File:   main.cpp
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:24 PM
 */

#include <cstdlib>
#include "Series.h"
#include "Trader.h"
#include "Operator/OperatorAdd.h"
#include "Operator/OperatorEma.h"
#include "Operator/OperatorSeries.h"
#include "Genetics/Generation.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    srand(time(NULL));
    
    Series seriesRI;    
    seriesRI.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140814_140814.txt");
    seriesRI.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140815_140815.txt");
    seriesRI.Normalize();
    
    Series seriesDJI;    
    seriesDJI.LoadFromFinamTickFile("../data/D&J-IND_140814_140814.txt");
    seriesDJI.LoadFromFinamTickFile("../data/D&J-IND_140815_140815.txt");
    seriesDJI.Normalize();
    
    vector<std::shared_ptr<Series>> generation_series;
    generation_series.push_back(std::make_shared<Series>(seriesRI));
    generation_series.push_back(std::make_shared<Series>(seriesDJI));
    Generation generation_test(10, generation_series);

    //Series seriesDJIEma100 = (seriesDJI/seriesDJI.SmaIndicator(100)).EmaIndicator(100);
    //Series seriesDJIEma1000 = (seriesDJI/seriesDJI.SmaIndicator(100)).EmaIndicator(1000);
    //Series seriesRIEma100 = (seriesRI/seriesRI.SmaIndicator(100)).EmaIndicator(100);
    //Series seriesRIEma1000 = (seriesRI/seriesRI.SmaIndicator(100)).EmaIndicator(1000);
    //Series seriesDiff = 2000.0 * (seriesRIEma100 - seriesRIEma1000 + seriesDJIEma100 - seriesDJIEma1000);


//    OperatorSeries seriesDJIEma100(std::make_shared<Series>((seriesDJI / seriesDJI.SmaIndicator(100))));
//    OperatorSeries seriesRIEma100(std::make_shared<Series>((seriesRI / seriesRI.SmaIndicator(100))));
//    OperatorAdd seriesAdd(std::make_shared<OperatorSeries>(seriesRIEma100), std::make_shared<OperatorSeries>(seriesDJIEma100));
//    OperatorEma seriesEma(std::make_shared<OperatorAdd>(seriesAdd), 1000);
//    std::shared_ptr<Operator> resultOperator = seriesEma.perform();
    
    /*TimeOfDay beginTod = {10, 0, 1};
    TimeOfDay endTod = {17, 0, 0};
    Series allowSeries = seriesRI.GenerateTradeAllowSingal(beginTod, endTod, 3600);

    Trader trader(seriesRI, seriesDiff, allowSeries, 10, 10, 2, 0.002);
    
    trader.Trade();
    
    vector<Series> plotSeries = {trader.GetTradeLimitBuy(), trader.GetTradeLimitSell(), seriesRI, 
        allowSeries * 1000.0 + 120000.0, seriesDiff * 1000 + 120000.0, 
        trader.GetTradePosition() * 1000 + 120000.0, trader.GetTradeAccount() + 120000.0};*/
    
    //vector<Series> plotSeries = {*dynamic_cast<OperatorSeries*>(resultOperator.get())->getSeries()};
    //Series::PlotGnu(1, plotSeries);

    return 0;
}

