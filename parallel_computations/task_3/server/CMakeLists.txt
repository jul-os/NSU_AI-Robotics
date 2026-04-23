cmake_minimum_required(VERSION 3.16)
project(task3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(server src/main.cpp)
add_executable(test_results src/test_results.cpp)

# Потоки
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)
target_link_libraries(test_results Threads::Threads)

# Оптимизация для релиза
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(server PRIVATE -O3 -march=native)
endif()