#include <gtest/gtest.h>
#include "Series.h"
#include "Helpers.h"

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
    DayOfTheYear Day1(localtime(&datetime1));
    DayOfTheYear Day3(localtime(&datetime1));
    EXPECT_EQ(2014 - 1900, Day1.year_corrected);
    EXPECT_EQ(225, Day1.day_of_year);

    long datetime2 = Helpers::GetTimeUtcFromTimezone(2014, 8, 15, 12, 30, 10, Helpers::Timezone::msk);
    DayOfTheYear Day2(localtime(&datetime2));
    EXPECT_EQ(2014 - 1900, Day2.year_corrected);
    EXPECT_EQ(226, Day2.day_of_year);
    
    EXPECT_EQ(true, Day1 < Day2);
    EXPECT_EQ(false, Day2 < Day1);
    EXPECT_EQ(true, Day3 == Day1);
    EXPECT_EQ(false, Day3 == Day2);
}

TEST(SeriesTest, SeriesOperations) {
    Series series;
    series.LoadFromFinamTickFile("../data/tests/SPFB.RTS-9.14_140814_140814_test.txt");
    series.LoadFromFinamTickFile("../data/tests/SPFB.RTS-9.14_140815_140815_test.txt");
    series.Normalize();
    EXPECT_EQ(121950, series.GetValue(
            Helpers::GetTimeUtcFromTimezone(2014, 8, 14, 11, 10, 30, Helpers::Timezone::msk)));
}