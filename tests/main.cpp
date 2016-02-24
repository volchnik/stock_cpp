#include <gtest/gtest.h>
#include "Series.h"
#include "Helpers.h"
#include "Trader.h"
#include "Generation.h"

TEST(SeriesTest, TimeHelpers) {
    tm time_structure;
    memset(&time_structure, 0, sizeof (tm));
    time_structure.tm_year = 2014 - 1900;
    time_structure.tm_mon = 8 - 1;
    time_structure.tm_mday = 14;
    time_structure.tm_hour = 11;
    time_structure.tm_min = 10;
    time_structure.tm_sec = 30;
    EXPECT_EQ(4, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));
    
    memset(&time_structure, 0, sizeof (tm));
    time_structure.tm_year = 2014 - 1900;
    time_structure.tm_mon = 11 - 1;
    time_structure.tm_mday = 27;
    time_structure.tm_hour = 11;
    time_structure.tm_min = 10;
    time_structure.tm_sec = 30;
    EXPECT_EQ(3, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));
    
    memset(&time_structure, 0, sizeof (tm));
    time_structure.tm_year = 2011 - 1900;
    time_structure.tm_mon = 6 - 1;
    time_structure.tm_mday = 14;
    time_structure.tm_hour = 11;
    time_structure.tm_min = 10;
    time_structure.tm_sec = 30;
    EXPECT_EQ(4, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));
    
    memset(&time_structure, 0, sizeof (tm));
    time_structure.tm_year = 2010 - 1900;
    time_structure.tm_mon = 11 - 1;
    time_structure.tm_mday = 28;
    time_structure.tm_hour = 11;
    time_structure.tm_min = 10;
    time_structure.tm_sec = 30;
    EXPECT_EQ(4, Helpers::GetTimezoneOffset(&time_structure, Helpers::Timezone::msk));
    
    memset(&time_structure, 0, sizeof (tm));
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
    Helpers::exec_bash("(cd ../data/finam_import/stock_quotes/data_seconds && find . -type d -exec mkdir -p ../data_seconds_decompressed/{} \\;)");
    Helpers::exec_bash("(cd ../data/finam_import/stock_quotes/data_seconds && find . -name \"*.txt\" -exec bash -c 'zcat < ${1} > \"../data_seconds_decompressed\"${1:1}' param {} \\;)");

    Series series("RtsTest");
    vector<SeriesSampleExtended> series_from_files;
    series.LoadFromFinamTickFile(series_from_files, "../data/finam_import/stock_quotes/data_seconds_decompressed/RI/RIU4_140814_140814.txt");
    series.LoadFromFinamTickFile(series_from_files, "../data/finam_import/stock_quotes/data_seconds_decompressed/RI/RIU4_140815_140815.txt");
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
    generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_ri.GetName(), std::make_shared<Series>(series_ri)));
    generation_series.insert(std::pair<std::string, std::shared_ptr<Series>>(series_dji.GetName(), std::make_shared<Series>(series_dji)));
    
    Trader trader(make_shared<Series>(series_ri), make_shared<Series>(series_ri), 10, 10, 2, 0.001);
    Generation generation(150, generation_series, trader, true);
    
    std::string operator_test_string_1("((SMA((-40.328799 + RI), 23.277791) / (-6.124357 / RI)) / (DJI - RI))");
    std::shared_ptr<Operator> operator_result_1 = generation.OperatorFromString(operator_test_string_1);
    EXPECT_EQ(operator_test_string_1, operator_result_1->ToString());
    
    std::string operator_test_string_2("((EMA((SMA(40.328799, 2.000000) + RI), 23.277791) * (-6.124357 / RI)) - (DJI + 3.000000))");
    std::shared_ptr<Operator> operator_result_2 = generation.OperatorFromString(operator_test_string_2);
    EXPECT_EQ(operator_test_string_2, operator_result_2->ToString());
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
