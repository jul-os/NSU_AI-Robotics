cmake_minimum_required(VERSION 3.14)
project(TableProcessor)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)

add_executable(table_test
    test.cpp
    table.cpp
)

target_link_libraries(table_test GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(table_test)