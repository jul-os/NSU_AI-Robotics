cmake_minimum_required(VERSION 3.10)
project(sine_sum C)

option(USE_FLOAT "Use float instead of double" OFF)

if(USE_FLOAT)
    add_compile_definitions(USE_FLOAT_TYPE)
    message(STATUS "Using float type")
else()
    message(STATUS "Using double type")
endif()

add_executable(sine_sum main.c)
target_link_libraries(sine_sum m)