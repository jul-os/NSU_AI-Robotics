cmake_minimum_required(VERSION 3.10)
project(MatrixTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Добавляем тесты
add_executable(matrix_test matrix_test.cpp)
target_sources(matrix_test PRIVATE matrix.cpp)

# Настройки компиляции
target_compile_options(matrix_test PRIVATE -Wall -Wextra -g)