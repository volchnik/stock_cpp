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
#include "../Operator/OperatorLog.h"
#include "../Genetics/Generation.h"
#include "../Genetics/FactorGeneration.h"

#include "../http/server_http.hpp"
#include "../http/client_http.hpp"

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

using namespace std;
using namespace boost::filesystem;
using namespace boost::adaptors;

boost::iterator_range<boost::filesystem::recursive_directory_iterator> traverse_directory(string const &dir) {
  boost::filesystem::recursive_directory_iterator f(dir), l;
  return boost::make_iterator_range(f, l);
}

static bool is_valid_file(boost::filesystem::directory_entry const &entry) {
  return is_regular_file(entry) && !is_other(entry);
}

static bool is_valid_dir(boost::filesystem::directory_entry const &entry) {
  return is_directory(entry) && !is_other(entry);
}

const static string kQuoteRepoPath = "../data/finam_import/stock_quotes";
const static string kQuotePath = "../data/finam_import/stock_quotes/data_seconds_decompressed";

/*
 *
 */
bool initialize_quote_data(series_map_type &series_map, series_ptr_map_type &generation_series, Trader *&ptrader,
                           std::shared_ptr<Series> &allowSeries, string quote_hash = "") {
  /*if (quote_hash != "") {
      Helpers::exec_bash("(cd " + kQuoteRepoPath + ")");
      string quote_hash_local = Helpers::exec_bash("git rev-parse HEAD");
      if (quote_hash != quote_hash_local) {

      } else {
          return true;
      }
  }*/
  Helpers::exec_bash("rm -rf ../data/finam_import/stock_quotes/data_seconds_decompressed/");
  Helpers::exec_bash(
      "(cd ../data/finam_import/stock_quotes/data_seconds && find . -type d -exec mkdir -p ../data_seconds_decompressed/{} \\;)");
  Helpers::exec_bash(
      "(cd ../data/finam_import/stock_quotes/data_seconds && find . -name \"*.txt\" -exec bash -c 'zcat < ${1} > \"../data_seconds_decompressed\"${1:1}' param {} \\;)");

  try {
    for (const auto &entry : traverse_directory(kQuotePath) | filtered(is_valid_dir)) {
      std::vector<string> string_split;
      boost::split(string_split, entry.path().string(), boost::is_any_of("//"));
      series_map.insert(pair<string, Series>(string_split.at(string_split.size() - 1),
                                             Series(string_split.at(string_split.size() - 1))));
    }
    for (auto &series : series_map) {
      vector<string> filename_list;
      for (const auto &entry : traverse_directory(kQuotePath + "/" + series.second.GetName()) | filtered(is_valid_file)) {
        filename_list.push_back(entry.path().string());
      }
      sort(filename_list.begin(), filename_list.end(), [](string first, string second) -> bool {
        return first.substr(first.size() - 17) < second.substr(second.size() - 17);
      });
      vector<SeriesSampleExtended> series_from_files;
      for (const string &filename : filename_list) {
        series.second.LoadFromFinamTickFile(series_from_files, filename);
        cout << filename << "\n";
      }
      series.second.Normalize(series_from_files);
    }
  } catch (const filesystem_error &e) {
    cout << "Error occured during quotes initialiaztion: " << e.what();
    return false;
  }

  for (auto &series : series_map) {
    if (series.second.GetName().find('_') == string::npos && series.second.GetName().compare("RTSI") != 0) {
      Series series_local = (series.second - series.second.SmaIndicator(120)) / (0.001 * series.second.SmaIndicator(120));
      series_local.SetName(series.second.GetName());
      generation_series.insert(pair<string, shared_ptr<Series>>(series.second.GetName(), make_shared<Series>(series_local)));
    }
  }

//    Series series_local = 1.0 - (0.0005*((series_map.find("RI")->second - series_map.find("RTSI")->second * 100.0).EmaIndicator(3600) -
  //      (series_map.find("RI")->second - series_map.find("RTSI")->second * 100.0).EmaIndicator(6*86000)).EmaIndicator(24*3600)).AtanIndicator();
//    generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_local.GetName(),
//                                                                             std::make_shared<Series>(series_local)));

  TimeOfDay beginTod = {10, 10, 1};
  TimeOfDay endTod = {17, 30, 0};
  allowSeries = make_shared<Series>(series_map.find("RI")->second.GenerateTradeAllowSingal(beginTod, endTod, 1800));

  ptrader = new Trader(make_shared<Series>(series_map.find("RI")->second),
                       make_shared<Series>(series_map.find("RI_MIN")->second),
                       make_shared<Series>(series_map.find("RI_MAX")->second),
                       make_shared<Series>(series_map.find("Si")->second),
                       allowSeries,
                       60, // Timeout after deal sec
                       10, // Update limit deal interval sec
                       1, // Max trade position
                       200, // Величина спреда для совершения сделок по сигналу (абсолютный дифференс)
                       0.95, // Stop loss level
                       86400 // Start offset
  );
  return true;
}
series_ptr_map_type getSubSeriesMap(const series_ptr_map_type &series_map, ulong offset, ulong interval) {

  series_ptr_map_type return_map;

  for (auto &series : series_map) {
    return_map.insert(pair<string, shared_ptr<Series>>(series.first, series.second->getSubSeries(offset, interval)));
  }

  return return_map;
}

/*
 *
 */
int main(int argc, char **argv) {

  srand(time(NULL));

  series_map_type series_map;
  Trader *ptrader = nullptr;
  series_ptr_map_type generation_series;

  std::shared_ptr<Series> allowSeries;

  initialize_quote_data(series_map, generation_series, ptrader, allowSeries);

  vector<std::shared_ptr<Series>> generation_series1;

  /*int day_offset = 1;
  int day_len = 5;*/
  series_ptr_map_type current_series(generation_series);//getSubSeriesMap(generation_series, day_offset, day_len);

  /*ptrader->SetStock(series_map.find("RI")->second.getSubSeries(day_offset, day_len),
                    series_map.find("RI_MIN")->second.getSubSeries(day_offset, day_len),
                    series_map.find("RI_MAX")->second.getSubSeries(day_offset, day_len),
                    series_map.find("Si")->second.getSubSeries(day_offset, day_len));*/

  HttpServer server(8080, 4);

  //GET-example for the path /info
  //Responds with request-information
  server.resource["^/info$"]["GET"] = [&series_map, &generation_series, &ptrader, &current_series](HttpServer::Response &response,
                                                                                                   shared_ptr<HttpServer::Request> request) {
    stringstream content_stream;
    content_stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port
                   << ")</h1>";
    content_stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";
    for (auto &header: request->header) {
      content_stream << header.first << ": " << header.second << "<br>";
    }

    //find length of content_stream (length received using content_stream.tellp())
    content_stream.seekp(0, ios::end);

    if (request->header.find("quote_hash") != request->header.end()
        && request->header.find("strategy_string") != request->header.end()) {

      Generation generation(1, current_series, *ptrader, true);

      std::shared_ptr<Operator>
          operatorStrategy = Operator::OperatorFromString(generation_series, request->header.find("strategy_string")->second);
      std::ostringstream strs;

      int numRepeatTrade = 1;
      double fitnessSum = 0.0;
      bool plotTrade = false;

      for (int i = 0; i < numRepeatTrade; i++) {
        std::tuple<double, Series, Series, Series, Series, Series, Series> result = ptrader->Trade(operatorStrategy);

        if (plotTrade) {
          vector<Series> plotSeries = {std::get<2>(result)};
          Series::GenerateCharts("plot_account.csv", Series::ChartsFormat::plotly, 1, plotSeries, 1);

          plotSeries = {std::get<6>(result)};
          Series::GenerateCharts("plot_signal.csv", Series::ChartsFormat::plotly, 1, plotSeries, 1);

          plotSeries = {series_map.find("RI")->second};
          Series::GenerateCharts("plot_ri.csv", Series::ChartsFormat::plotly, 1, plotSeries, 1);
        }

        fitnessSum += Generation::GetStrategyFitness(operatorStrategy, *ptrader);

      }
      strs << fitnessSum / (double)numRepeatTrade;
      std::string sOutput = strs.str();

      response << "HTTP/1.1 200 OK\r\nContent-Length: " << sOutput.length() << "\r\n\r\n" << sOutput;
    } else {
      response << "HTTP/1.1 400 Bad Request\r\n";
      return;
    }
  };

  thread server_thread([&server]() {
    server.start();
  });

  /*while (true) {
    sleep(1);
  }*/

  // return 0;

  //series_ptr_map_type current_series = generation_series;

  //series_ptr_map_type current_series = generation_series;
  //ptrader->SetStock(current_series.find("RI")->second);

  Generation generation_test(150, generation_series, *ptrader, true);

  generation_test.GenerateRandomSeed();

  int generation_counter = 0;
  bool recalc_generation = false;

  while (++generation_counter) {
    cout << "Iteration: " << generation_counter << endl;
    generation_test.IterateGeneration(recalc_generation);
    recalc_generation = false;

    if (generation_counter % 10 == 0) {
      for (unsigned long index = 0; index < floor((double) generation_test.GetGenerationCount() / 2.0); index += 60) {
        FactorGeneration
            factor_generation(generation_test.GetStrategy(index).first, generation_test.GetStrategy(index).second, 24, *ptrader);

        factor_generation.GenerateRandomSeed();
        for (int fi = 0; fi < 16; fi++) {
          generation_test.UpdateStrategy(index,
                                         pair<shared_ptr<Operator>, double>(generation_test.GetStrategy(index).first,
                                                                            factor_generation.IterateGeneration()));
        }

        std::shared_ptr<Operator> operator_optimized = generation_test.GetStrategy(index).first;
        Operator::SimplifyOperator(operator_optimized);

        cout << "Optimization result: " << generation_test.GetStrategy(index).first->ToString() << " "
             << generation_test.GetStrategy(index).second << std::endl;
      }

      generation_test.SortStrategy();
    }

    std::tuple<double, Series, Series, Series, Series, Series, Series>
        result = ptrader->Trade(generation_test.GetLeaderStrategy().first);

    OperatorAdd resultOperatorAdd
        (std::make_shared<OperatorSeries>(std::make_shared<Series>(series_map.find("RI")->second.GenerateZeroBaseSeries())),
         generation_test.GetLeaderStrategy().first);
    std::shared_ptr<Operator> resultOperator = resultOperatorAdd.perform();

  }
    
//    vector<Series> plotSeries = {trader.GetTradeLimitBuy(), trader.GetTradeLimitSell(), seriesRI, 
//        allowSeries * 1000.0 + 120000.0, signal_series * 1000 + 120000.0, 
//        trader.GetTradePosition() * 1000 + 120000.0, trader.GetTradeAccount() + 120000.0};
    
    //vector<Series> plotSeries = {*dynamic_cast<OperatorSeries*>(resultOperator.get())->getSeries()};
//    Series::PlotGnu(1, plotSeries);
    
    return 0;
}

