cmake_minimum_required(VERSION 3.14)
project(SmartKettle LANGUAGES CXX)

# Требуем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключаем Google Test через FetchContent
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Совместимость с MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Список заголовочных файлов (необязательно, но для ясности)
set(HEADER_FILES
    Kettle.h
    HeatingStrategy.h
    NotificationChannel.h
    NotificationManager.h
)

# Основной исполняемый файл
add_executable(kettle main.cpp ${HEADER_FILES})

# Тестовый исполняемый файл
add_executable(kettle_tests kettle_tests.cpp ${HEADER_FILES})
target_link_libraries(kettle_tests gtest_main gtest)
target_include_directories(kettle_tests PRIVATE .)

# Регистрируем тест в CTest
enable_testing()
add_test(NAME kettle_tests COMMAND kettle_tests)