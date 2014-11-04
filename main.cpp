/* 
 * File:   main.cpp
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:24 PM
 */

#include <cstdlib>
#include "Series.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Series series;
    
    series.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140814_140814.txt");
    series.LoadFromFinamTickFile("../data/SPFB.RTS-9.14_140815_140815.txt");
    series.Normalize();
    return 0;
}

