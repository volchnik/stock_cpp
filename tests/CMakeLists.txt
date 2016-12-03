project(test_stock_cpp)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lib/googletest)

find_package(Boost COMPONENTS coroutine thread system filesystem regex REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()
include_directories(lib/googletest/include/gtest lib/googletest)

add_executable(test_stock_cpp main.cpp)

target_link_libraries(test_stock_cpp gtest gtest_main)
target_link_libraries(test_stock_cpp stock_cpp_lib)
if(Boost_FOUND)
    target_link_libraries(test_stock_cpp ${Boost_LIBRARIES})
endif()
