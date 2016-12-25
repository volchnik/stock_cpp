#include <gtest.h>
#include <gtest_pred_impl.h>
#include "../Series.h"
#include "../Helpers.h"
#include "../Trader.h"
#include "../Genetics/Generation.h"
#include "../Operator/Operator.h"
#include "../Operator/OperatorBinary.h"

TEST(SeriesTest, TimeHelpers) {
  tm time_structure;
  memset(&time_structure, 0, sizeof(tm));
  time_structure.tm_year = 2014 - 1900;
  time_structure.tm_mon = 8 - 1;
  time_structure.tm_mday = 14;
  time_structure.tm_hour = 11;
  time_structure.tm_min = 10;
  time_structure.tm_sec = 30;
  EXPECT_EQ(4, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));

  memset(&time_structure, 0, sizeof(tm));
  time_structure.tm_year = 2014 - 1900;
  time_structure.tm_mon = 11 - 1;
  time_structure.tm_mday = 27;
  time_structure.tm_hour = 11;
  time_structure.tm_min = 10;
  time_structure.tm_sec = 30;
  EXPECT_EQ(3, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));

  memset(&time_structure, 0, sizeof(tm));
  time_structure.tm_year = 2011 - 1900;
  time_structure.tm_mon = 6 - 1;
  time_structure.tm_mday = 14;
  time_structure.tm_hour = 11;
  time_structure.tm_min = 10;
  time_structure.tm_sec = 30;
  EXPECT_EQ(4, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));

  memset(&time_structure, 0, sizeof(tm));
  time_structure.tm_year = 2010 - 1900;
  time_structure.tm_mon = 11 - 1;
  time_structure.tm_mday = 28;
  time_structure.tm_hour = 11;
  time_structure.tm_min = 10;
  time_structure.tm_sec = 30;
  EXPECT_EQ(4, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));

  memset(&time_structure, 0, sizeof(tm));
  time_structure.tm_year = 2009 - 1900;
  time_structure.tm_mon = 6 - 1;
  time_structure.tm_mday = 28;
  time_structure.tm_hour = 11;
  time_structure.tm_min = 10;
  time_structure.tm_sec = 30;
  EXPECT_EQ(3, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));

  EXPECT_EQ(1408014610 - 3600 * 4,
            Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 11, 10, 10, Helpers::Timezone::msk));

  EXPECT_EQ(1416049810 - 3600 * 3,
            Helpers::GetTimeUtcFromTimezone(2014, 11, 15, 11, 10, 10, Helpers::Timezone::msk));
}

TEST(SeriesTest, DayOfYear) {
  long datetime1 = Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 11, 10, 30, Helpers::Timezone::msk);
  DayOfTheYear Day1(gmtime(&datetime1));
  DayOfTheYear Day3(gmtime(&datetime1));
  EXPECT_EQ(2014 - 1900, Day1.year_corrected);
  EXPECT_EQ(225, Day1.day_of_year);

  long datetime2 = Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 12, 30, 10, Helpers::Timezone::msk);
  DayOfTheYear Day2(gmtime(&datetime2));
  EXPECT_EQ(2014 - 1900, Day2.year_corrected);
  EXPECT_EQ(226, Day2.day_of_year);

  EXPECT_EQ(true, Day1 < Day2);
  EXPECT_EQ(false, Day2 < Day1);
  EXPECT_EQ(true, Day3 == Day1);
  EXPECT_EQ(false, Day3 == Day2);
}

TEST(SeriesTest, SeriesOperations) {
  Helpers::exec_bash("rm -rf ../data/finam_import/stock_quotes/data_seconds_decompressed/");
  Helpers::exec_bash(
      "(cd ../data/finam_import/stock_quotes/data_seconds && find . -type d -exec mkdir -p ../data_seconds_decompressed/{} \\;)");
  Helpers::exec_bash(
      "(cd ../data/finam_import/stock_quotes/data_seconds && find . -name \"*.txt\" -exec bash -c 'zcat < ${1} > \"../data_seconds_decompressed\"${1:1}' param {} \\;)");

  Series series("RtsTest");
  vector<SeriesSampleExtended> series_from_files;
  series.LoadFromFinamTickFile(series_from_files, "../data/tests/RI/RIU4_140814_140814.txt");
  series.LoadFromFinamTickFile(series_from_files, "../data/tests/RI/RIU4_140815_140815.txt");
  series.Normalize(series_from_files);
  EXPECT_EQ(121950, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 11, 10, 30, Helpers::Timezone::msk)));
  EXPECT_EQ(123120, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 14, 31, 46, Helpers::Timezone::msk)));
  EXPECT_EQ(123200, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 14, Helpers::Timezone::msk)));
  EXPECT_EQ(123110, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(122500, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 23, 49, 58, Helpers::Timezone::msk)));
  EXPECT_EQ(122660, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 10, 0, 0, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));
  EXPECT_EQ(123810, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 57, Helpers::Timezone::msk)));
  EXPECT_EQ(122500, series.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 06, 00, 00, Helpers::Timezone::msk)));

  Series seriesCopy = series;
  EXPECT_EQ(123110, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy = series;
  seriesCopy.SetValue(Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk), 100000);
  EXPECT_EQ(100000, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));

  seriesCopy = series;
  EXPECT_EQ(123110, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  Series seriesCopy1(series);
  EXPECT_EQ(123110, seriesCopy1.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, seriesCopy1.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy += series;
  EXPECT_EQ(246220, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(247560, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy -= series;
  EXPECT_EQ(123110, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  Series seriesCopy2(series + series);
  EXPECT_EQ(246220, seriesCopy2.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(247560, seriesCopy2.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  Series seriesCopy3(series - series);
  EXPECT_EQ(0, seriesCopy3.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(0, seriesCopy3.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy = series;
  seriesCopy *= 2.0;
  EXPECT_EQ(246220, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(247560, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy /= 2.0;
  EXPECT_EQ(123110, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy = seriesCopy * 2.0;
  EXPECT_EQ(246220, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(247560, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy = seriesCopy / 2.0;
  EXPECT_EQ(123110, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123780, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy = 2.0 * seriesCopy;
  EXPECT_EQ(246220, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(247560, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  /* seriesCopy = series.EmaIndicator(1000);
   EXPECT_NEAR(123110, seriesCopy.GetValue(
                                           Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)), 200);
   EXPECT_NEAR(123780, seriesCopy.GetValue(
                                           Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)), 200);
   EXPECT_NEAR(122660, seriesCopy.GetValue(
                                           Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 10, 0, 1, Helpers::Timezone::msk)), 200);
   */
  seriesCopy = series.SmaIndicator(3);
  EXPECT_DOUBLE_EQ(123780, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));
  EXPECT_NEAR(123646.66, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 31, 51, Helpers::Timezone::msk)), 0.01);

  seriesCopy = series + 1.0;
  EXPECT_EQ(123111, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123781, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy = 1.0 + series;
  EXPECT_EQ(123111, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123781, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));

  seriesCopy += 1.0;
  EXPECT_EQ(123112, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 17, 42, 31, Helpers::Timezone::msk)));
  EXPECT_EQ(123782, seriesCopy.GetValue(
      Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 15, 28, 44, Helpers::Timezone::msk)));
}

TEST(SeriesTest, OperatorEncodeDecode) {
  Series series_ri("RI");
  Series series_dji("DJI");

  map<std::string, std::shared_ptr<Series>> generation_series;
  generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_ri.GetName(),
                                                                           std::make_shared<Series>(series_ri)));
  generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_dji.GetName(),
                                                                           std::make_shared<Series>(series_dji)));

  std::string operator_test_string_1("((SMA((40.328799 + RI), 23.277791) / (6.124357 / RI)) / (DJI - RI))");
  std::shared_ptr<Operator> operator_result_1 = Operator::OperatorFromString(generation_series, operator_test_string_1);
  EXPECT_EQ(operator_test_string_1, operator_result_1->ToString());

  std::string operator_test_string_2("((EMA((SMA(40.328799, 2.000000) + RI), 23.277791) * (6.124357 / RI)) - (DJI + 3.000000))");
  std::shared_ptr<Operator> operator_result_2 = Operator::OperatorFromString(generation_series, operator_test_string_2);
  EXPECT_EQ(operator_test_string_2, operator_result_2->ToString());
}

TEST(SeriesTest, OperatorSimplify) {
  Series series_ri("RI");
  Series series_dji("DJI");

  map<std::string, std::shared_ptr<Series>> generation_series;
  generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_ri.GetName(),
                                                                           std::make_shared<Series>(series_ri)));
  generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_dji.GetName(),
                                                                           std::make_shared<Series>(series_dji)));

  std::string operator_test_string_1("EMA(10.0, 20.0)");
  std::shared_ptr<Operator> operator_result_1 = Operator::OperatorFromString(generation_series, operator_test_string_1);
  Operator::SimplifyOperator(operator_result_1);
  EXPECT_EQ("10.000000", operator_result_1->ToString());

  std::string operator_test_string_2("RI - RI");
  std::shared_ptr<Operator> operator_result_2 = Operator::OperatorFromString(generation_series, operator_test_string_2);
  Operator::SimplifyOperator(operator_result_2);
  EXPECT_EQ("0.000000", operator_result_2->ToString());

  std::string operator_test_string_3("DJI - (SMA((RI - RI), 1.0))");
  std::shared_ptr<Operator> operator_result_3 = Operator::OperatorFromString(generation_series, operator_test_string_3);
  Operator::SimplifyOperator(operator_result_3);
  EXPECT_EQ("DJI", operator_result_3->ToString());

  std::string operator_test_string_4("DJI - (0.0 * (SMA((DJI - RI), 1.0)))");
  std::shared_ptr<Operator> operator_result_4 = Operator::OperatorFromString(generation_series, operator_test_string_4);
  Operator::SimplifyOperator(operator_result_4);
  EXPECT_EQ("DJI", operator_result_4->ToString());

  std::string operator_test_string_5("EMA(RI, 20.5)");
  std::shared_ptr<Operator> operator_result_5 = Operator::OperatorFromString(generation_series, operator_test_string_5);
  Operator::SimplifyOperator(operator_result_5);
  EXPECT_EQ("EMA(RI, 21.000000)", operator_result_5->ToString());

  std::string operator_test_string_6("EMA(RI, 22.3)");
  std::shared_ptr<Operator> operator_result_6 = Operator::OperatorFromString(generation_series, operator_test_string_6);
  Operator::SimplifyOperator(operator_result_6);
  EXPECT_EQ("EMA(RI, 22.000000)", operator_result_6->ToString());

  std::string operator_test_string_7("EMA(RI, 0.3)");
  std::shared_ptr<Operator> operator_result_7 = Operator::OperatorFromString(generation_series, operator_test_string_7);
  Operator::SimplifyOperator(operator_result_7);
  EXPECT_EQ("RI", operator_result_7->ToString());

  std::string operator_test_string_8("EMA(RI, 0.3)");
  std::shared_ptr<Operator> operator_result_8 = Operator::OperatorFromString(generation_series, operator_test_string_8);
  Operator::SimplifyOperator(operator_result_8);
  EXPECT_EQ("RI", operator_result_8->ToString());

  std::string operator_test_string_9("SMA((6.517391 * (2.302582 * RI)), 52.011556)");
  std::shared_ptr<Operator> operator_result_9 = Operator::OperatorFromString(generation_series, operator_test_string_9);
  Operator::SimplifyOperator(operator_result_9);
  EXPECT_EQ("SMA((6.517391 * (2.302582 * RI)), 52.000000)", operator_result_9->ToString());
}

TEST(SeriesTest, OperatorCrossingover) {
  Series series_ri("RI");
  Series series_dji("DJI");

  map<std::string, std::shared_ptr<Series>> generation_series;
  generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_ri.GetName(),
                                                                           std::make_shared<Series>(series_ri)));
  generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_dji.GetName(),
                                                                           std::make_shared<Series>(series_dji)));

  std::string operator_test_string_1("((SMA((40.328799 + RI), 23.277791) / (6.124357 / RI)) / (DJI - RI))");
  std::shared_ptr<Operator> operator_result_1 = Operator::OperatorFromString(generation_series, operator_test_string_1);

  std::string operator_test_string_2("((EMA((SMA(40.328799, 2.000000) + RI), 23.277791) * (6.124357 / RI)) - (DJI + 3.000000))");
  std::shared_ptr<Operator> operator_result_2 = Operator::OperatorFromString(generation_series, operator_test_string_2);

  std::shared_ptr<Operator> &operator_cross_1 = (dynamic_cast<OperatorBinary *>(operator_result_1.get()))->GetOperatorRight();
  std::shared_ptr<Operator> &operator_cross_2 = (dynamic_cast<OperatorBinary *>(operator_result_2.get()))->GetOperatorLeft();
  operator_cross_1.swap(operator_cross_2);
  EXPECT_EQ(
      "((SMA((40.328799 + RI), 23.277791) / (6.124357 / RI)) / (EMA((SMA(40.328799, 2.000000) + RI), 23.277791) * (6.124357 / RI)))",
      operator_result_1->ToString());
}

TEST(TraderTest, GetCurrentSignal) {
  double limit_buy_level_fix_deal = 0.0;
  double limit_sell_level_fix_deal = 0.0;
  long timeout_after_deal_seconds = 0;
  long update_level_cooldown_seconds = 0;

  Trader::operationType testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 89, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::STAY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 91, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 2, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  EXPECT_EQ(1, timeout_after_deal_seconds);

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 91, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::STAY, testOperationType);
  EXPECT_EQ(0, timeout_after_deal_seconds);

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 91, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  EXPECT_EQ(9, timeout_after_deal_seconds);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 90, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 0, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  EXPECT_EQ(0, timeout_after_deal_seconds);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 1, 90, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::STAY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, -10, 90, 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 80, 111, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::STAY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 80, 109, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 2, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::SELL, testOperationType);
  EXPECT_EQ(1, timeout_after_deal_seconds);

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 80, 110, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::STAY, testOperationType);
  EXPECT_EQ(0, timeout_after_deal_seconds);

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 80, 110, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::SELL, testOperationType);
  EXPECT_EQ(9, timeout_after_deal_seconds);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 80, 100, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 0, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::SELL, testOperationType);
  EXPECT_EQ(0, timeout_after_deal_seconds);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, -1, 80, 100, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::STAY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 10, 80, 100, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::SELL, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 90, 110, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 100, 100, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 10, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, numeric_limits<double>::max(), 120, limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 2, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::BUY, testOperationType);
  EXPECT_EQ(1, timeout_after_deal_seconds);
  timeout_after_deal_seconds = 0;

  testOperationType =
      Trader::GetCurrentSignal(90, 110, 0, 80, -numeric_limits<double>::max(), limit_buy_level_fix_deal, limit_sell_level_fix_deal,
                               timeout_after_deal_seconds, 2, update_level_cooldown_seconds, 1);
  EXPECT_EQ(Trader::operationType::SELL, testOperationType);
  EXPECT_EQ(1, timeout_after_deal_seconds);
  timeout_after_deal_seconds = 0;
}

TEST(TraderTest, GetLimitDealLevels) {
  long datetime = Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 11, 10, 30, Helpers::Timezone::msk);
  DayOfTheYear day_of_the_year(gmtime(&datetime));
  std::shared_ptr<DayOfTheYear> day_of_the_year_ptr = std::make_shared<DayOfTheYear>(day_of_the_year);

  Series series("RtsTest");
  vector<SeriesSampleExtended> series_from_files;
  series.LoadFromFinamTickFile(series_from_files, "../data/tests/RI/RIU4_140814_140814.txt");
  series.Normalize(series_from_files);
  TimeOfDay beginTod = {10, 10, 1};
  TimeOfDay endTod = {17, 30, 0};
  std::shared_ptr<Series> allowSeries = make_shared<Series>(series.GenerateTradeAllowSingal(beginTod, endTod, 1800));
  Series trade_limit_buy(series);
  Series trade_limit_sell(series);

  long update_level_cooldown_seconds = 0;
  pair<double, double> limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, 2.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(numeric_limits<double>::max(), limits.second);

  update_level_cooldown_seconds = 0;
  limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, 2000.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(numeric_limits<double>::max(), limits.second);

  update_level_cooldown_seconds = 0;
  limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, 1.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(numeric_limits<double>::max(), limits.second);

  update_level_cooldown_seconds = 0;
  limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, 0.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(numeric_limits<double>::max(), limits.second);

  update_level_cooldown_seconds = 0;
  limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, -1.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(numeric_limits<double>::max(), limits.second);

  update_level_cooldown_seconds = 0;
  limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, -2.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.second);

  update_level_cooldown_seconds = 0;
  limits =
      Trader::GetLimitDealLevels(day_of_the_year_ptr, datetime, 0, -2000.0, 10000.0, 0, 10, update_level_cooldown_seconds,
                                 allowSeries, trade_limit_buy, trade_limit_sell, false);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.first);
  EXPECT_EQ(-numeric_limits<double>::max(), limits.second);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
