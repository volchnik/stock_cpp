cmake_minimum_required(VERSION 3.3)
project(stock_cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(SOURCE_FILES
    Genetics/FactorGeneration.cpp
    Genetics/FactorGeneration.h
    Genetics/Generation.cpp
    Genetics/Generation.h
    http/client_http.hpp
    http/server_http.hpp
    Operator/Operator.cpp
    Operator/Operator.h
    Operator/OperatorAdd.cpp
    Operator/OperatorAdd.h
    Operator/OperatorBinary.cpp
    Operator/OperatorBinary.h
    Operator/OperatorDivide.cpp
    Operator/OperatorDivide.h
    Operator/OperatorEma.cpp
    Operator/OperatorEma.h
    Operator/OperatorMultiply.cpp
    Operator/OperatorMultiply.h
    Operator/OperatorNumber.cpp
    Operator/OperatorNumber.h
    Operator/OperatorSeries.cpp
    Operator/OperatorSeries.h
    Operator/OperatorSma.cpp
    Operator/OperatorSma.h
    Operator/OperatorSubtract.cpp
    Operator/OperatorSubtract.h
    Operator/OperatorUnary.cpp
    Operator/OperatorUnary.h
    Series/TimeOfDay.h
    stock_cpp/main.cpp
    DayOfTheYear.h
    Helpers.cpp
    Helpers.h
    root.h
    Series.cpp
    Series.h
    SeriesInterval.h
    SeriesSample.h
    SeriesSampleExtended.cpp
    SeriesSampleExtended.h
    Strategy.cpp
    Strategy.h
    ThreadPool.h
    Trader.cpp
    Trader.h
    TraderOperation.cpp
    TraderOperation.h)

add_definitions(-DBOOST_RANGE_ENABLE_CONCEPT_ASSERT=0)

find_package(Boost COMPONENTS system filesystem regex coroutine context thread REQUIRED)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(stock_cpp ${SOURCE_FILES})

if(Boost_FOUND)

    target_link_libraries(stock_cpp ${Boost_LIBRARIES})

endif()