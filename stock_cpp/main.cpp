/* 
 * File:   main.cpp
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:24 PM
 */

#include <cstdlib>
#include "../Helpers.h"
#include "../Series.h"
#include "../Trader.h"
#include "../Operator/OperatorAdd.h"
#include "../Operator/OperatorEma.h"
#include "../Operator/OperatorSeries.h"
#include "../Genetics/Generation.h"
#include "../Genetics/FactorGeneration.h"

#include "../http/server_http.hpp"
#include "../http/client_http.hpp"

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

using namespace std;
using namespace boost::filesystem;
using namespace boost::adaptors;

boost::iterator_range<boost::filesystem::recursive_directory_iterator> traverse_directory(string const& dir) {
    boost::filesystem::recursive_directory_iterator f(dir), l;
    return boost::make_iterator_range(f, l);
}

static bool is_valid_file(boost::filesystem::directory_entry const& entry) {
    return is_regular_file(entry) && !is_other(entry);
}

static bool is_valid_dir(boost::filesystem::directory_entry const& entry) {
    return is_directory(entry) && !is_other(entry);
}

const static string kQuoteRepoPath = "../data/finam_import/stock_quotes";
const static string kQuotePath = "../data/finam_import/stock_quotes/data_seconds_decompressed";
typedef map<string, Series> series_map_type;
typedef map<string, shared_ptr<Series>> series_ptr_map_type;

/*
 *
 */
bool initialize_quote_data(series_map_type& series_map, series_ptr_map_type& generation_series, Trader*& ptrader, string quote_hash = "") {
    if (quote_hash != "") {
        Helpers::exec_bash("(cd " + kQuoteRepoPath + ")");
        string quote_hash_local = Helpers::exec_bash("git rev-parse HEAD");
        if (quote_hash != quote_hash_local) {
            
        } else {
            return true;
        }
    }
    Helpers::exec_bash("rm -rf ../data/finam_import/stock_quotes/data_seconds_decompressed/");
    Helpers::exec_bash("(cd ../data/finam_import/stock_quotes/data_seconds && find . -type d -exec mkdir -p ../data_seconds_decompressed/{} \\;)");
    Helpers::exec_bash("(cd ../data/finam_import/stock_quotes/data_seconds && find . -name \"*.txt\" -exec bash -c 'zcat < ${1} > \"../data_seconds_decompressed\"${1:1}' param {} \\;)");
    
    try {
        for (const auto& entry : traverse_directory(kQuotePath) | filtered(is_valid_dir)) {
            std::vector<string> string_split;
            boost::split(string_split, entry.path().string(), boost::is_any_of("//"));
            series_map.insert(pair<string, Series>(string_split.at(string_split.size() - 1), Series(string_split.at(string_split.size() - 1))));
        }
        for (auto& series : series_map) {
            vector<string> filename_list;
            for (const auto& entry : traverse_directory(kQuotePath + "/" + series.second.GetName()) | filtered(is_valid_file)) {
                filename_list.push_back(entry.path().string());
            }
            sort(filename_list.begin(), filename_list.end(), [](string first, string second) -> bool
                 {
                     return first.substr(first.size() - 17) < second.substr(second.size() - 17);
                 });
            vector<SeriesSampleExtended> series_from_files;
            for (const string& filename : filename_list) {
                series.second.LoadFromFinamTickFile(series_from_files, filename);
                cout << filename << "\n";
            }
            series.second.Normalize(series_from_files);
        }
    } catch(const filesystem_error& e) {
        cout << "Error occured during quotes initialiaztion: " << e.what();
        return false;
    }

    generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_map.find("RI")->second.GetName(),
                                                                             std::make_shared<Series>(series_map.find("RI")->second/series_map.find("RI")->second.SmaIndicator(100.0))));
    generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_map.find("YM")->second.GetName(),
                                                                             std::make_shared<Series>(series_map.find("YM")->second/series_map.find("YM")->second.SmaIndicator(100.0))));
    generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_map.find("SPFB.Si")->second.GetName(),
                                                                             std::make_shared<Series>(series_map.find("SPFB.Si")->second/series_map.find("SPFB.Si")->second.SmaIndicator(100.0))));
    TimeOfDay beginTod = {10, 0, 1};
    TimeOfDay endTod = {17, 0, 0};
    std::shared_ptr<Series> allowSeries = make_shared<Series>(series_map.find("RI")->second.GenerateTradeAllowSingal(beginTod, endTod, 3600));
    
    ptrader = new Trader(make_shared<Series>(series_map.find("RI")->second), allowSeries, 10, 10, 2, 0.001, 0.05);
    return true;
}

/*
 *
 */
int main(int argc, char** argv) {
    
    srand(time(NULL));
    
    series_map_type series_map;
    Trader* ptrader = nullptr;
    series_ptr_map_type generation_series;
    initialize_quote_data(series_map, generation_series, ptrader);

    vector<std::shared_ptr<Series>> generation_series1;
    
    HttpServer server(8080, 4);
    
    //GET-example for the path /info
    //Responds with request-information
    server.resource["^/info$"]["GET"]=[&series_map, &generation_series, &ptrader](HttpServer::Response& response, shared_ptr<HttpServer::Request> request) {
        stringstream content_stream;
        content_stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
        content_stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";
        for(auto& header: request->header) {
            content_stream << header.first << ": " << header.second << "<br>";
        }
        
        //find length of content_stream (length received using content_stream.tellp())
        content_stream.seekp(0, ios::end);
        
        if (request->header.find("quote_hash") != request->header.end()) {
            //initialize_quote_data(series_map, generation_series, ptrader, request->header.find("quote_hash")->second);
            
            map<string, Series> series_map;
            
            Generation generation(150, generation_series, *ptrader, true);
            
            std::shared_ptr<Operator> operatorTest = generation.OperatorFromString("(SMA((RI - YM), 2.277791) / (EMA((RI - SPFB.Si), 100)) + (SPFB.Si - YM) - (EMA((SPFB.Si - YM), 50)))");
            //std::shared_ptr<Operator> operatorTest = generation.OperatorFromString("(RI - YM)");
            std::ostringstream strs;
            strs << generation.GetStrategyFitness(operatorTest, *ptrader);
            std::string sOutput = strs.str();
            
            response <<  "HTTP/1.1 200 OK\r\nContent-Length: " << sOutput.length() << "\r\n\r\n" << sOutput;
        } else {
            response <<  "HTTP/1.1 400 Bad Request\r\n";
            return;
        }
    };
    
    /*thread server_thread([&server](){
        server.start();
    });
    
    while (true) {
        sleep(1);
    }

    return 0;*/
    
  /*  TimeOfDay beginTod = {10, 0, 1};
    TimeOfDay endTod = {17, 0, 0};
    Series allowSeries = series_map.find("RI")->second.GenerateTradeAllowSingal(beginTod, endTod, 3600);
    
    double scaleCoef = 1.0;
    
    Trader trader(series_map.find("RI")->second, allowSeries, 10, 10, 2, 0.001);
    */
    Generation generation_test(40, generation_series, *ptrader, true);
    
    /*std::shared_ptr<Operator> operatorTest = generation_test.OperatorFromString("(SMA((RI - RI), 10) - (EMA(42.240984, 65.346053) + (SPFB.Si + SPFB.Si)))");
    std::ostringstream strs;
    double res = generation_test.GetStrategyFitness(operatorTest);
    cout << res << endl;
    
    std::shared_ptr<Operator> operatorTest1 = generation_test.OperatorFromString("(0.0 - (EMA(42.240984, 65.346053) + (SPFB.Si + SPFB.Si)))");
    std::ostringstream strs1;
    double res1 = generation_test.GetStrategyFitness(operatorTest1);
    cout << res1 << endl;*/
    
    
    generation_test.GenerateRandomSeed();
    
    int generation_counter = 0;
    
    while(++generation_counter) {
        generation_test.IterateGeneration();
        
        if(generation_counter % 10 == 0) {
            for (unsigned long index = 0; index < generation_test.GetGenerationCount(); index+=((double)rand() / RAND_MAX * generation_test.GetGenerationCount() / 4.0)) {
                FactorGeneration factor_generation(generation_test.GetStrategy(index).first, generation_test.GetStrategy(index).second, 4, *ptrader);
                
                factor_generation.GenerateRandomSeed();
                for (int fi = 0; fi < 6; fi ++) {
                    generation_test.UpdateStrategy(index, pair<shared_ptr<Operator>, double>(generation_test.GetLeaderStrategy().first, factor_generation.IterateGeneration()));
                }
            }
        }
        
        std::tuple<double, Series, Series, Series, Series, Series> result = ptrader->Trade(generation_test.GetLeaderStrategy().first);
        //Series trade_series = std::get<2>(result);
        
        //std::shared_ptr<Operator> resultOperator = generation_test.GetLeaderStrategy().first->perform();
        
        vector<Series> plotSeries = {std::get<2>(result)};
        char fname[128];
        sprintf(fname, "plot_%d", generation_counter);
        Series::PlotGnu(fname, 1, plotSeries);
    }
    
   /* while(0) {
    std::shared_ptr<Operator> strategy = generation_test.GenerateRandom(5);
    string res = strategy->ToString();
    cout << res << endl << endl << endl;
    

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

    
    
        double trade_result = 0.0;
        std::tuple<double, Series, Series, Series, Series> result = trader.Trade(strategy);
        trade_result = std::get<0>(result);
        Series trade_series = std::get<2>(result);
    
        cout << trade_result << endl;
        cout << trade_series.CalculateBoundStatistic() << endl;
    }
    
//    vector<Series> plotSeries = {trader.GetTradeLimitBuy(), trader.GetTradeLimitSell(), seriesRI, 
//        allowSeries * 1000.0 + 120000.0, signal_series * 1000 + 120000.0, 
//        trader.GetTradePosition() * 1000 + 120000.0, trader.GetTradeAccount() + 120000.0};
    
    //vector<Series> plotSeries = {*dynamic_cast<OperatorSeries*>(resultOperator.get())->getSeries()};
//    Series::PlotGnu(1, plotSeries);
*/
    return 0;
}

