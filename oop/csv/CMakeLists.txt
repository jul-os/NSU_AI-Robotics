cmake_minimum_required(VERSION 3.14)  # gtest требует >=3.14

project(CSVparser LANGUAGES CXX)

# Установить стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Найти Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Для совместимости с Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Основной исполняемый файл
add_executable(main main.cpp)

# Тестовый исполняемый файл
add_executable(tests test.cpp)
target_link_libraries(tests gtest_main gtest)
target_include_directories(tests PRIVATE .)

# Включить тестирование
enable_testing()
add_test(NAME csvparser_tests COMMAND tests)