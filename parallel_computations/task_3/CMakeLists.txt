cmake_minimum_required(VERSION 3.16)
project(matrix_vector_mult LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

find_package(Threads REQUIRED)

add_executable(matrix_vector_mult matrix_vector_mult.cpp)
target_link_libraries(matrix_vector_mult PRIVATE Threads::Threads)

# Оптимизации для разных компиляторов
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(matrix_vector_mult PRIVATE -Wall -Wextra)
endif()