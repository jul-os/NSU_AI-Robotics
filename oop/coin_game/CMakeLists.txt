cmake_minimum_required(VERSION 3.14)
project(RobotCoinGame)

set(CMAKE_CXX_STANDARD 17)

# Находим пакет GTest
find_package(GTest REQUIRED)

# Основная программа
add_executable(robot_game 
    main.cpp 
    CoinGame.hpp 
    GameEngine.cpp GameEngine.hpp
    GameField.cpp GameField.hpp
    GameVisualizer.cpp GameVisualizer.hpp
    Robot.cpp Robot.hpp
    RobotFactory.cpp RobotFactory.hpp
    Position.hpp
    GameTypes.hpp
)

# Тесты
add_executable(robot_tests
    tests.cpp       # Ваши тесты
    GameEngine.cpp
    GameField.cpp
    GameVisualizer.cpp
    Robot.cpp
    RobotFactory.cpp
)

# Линкуем тесты с GTest
target_link_libraries(robot_tests GTest::gtest GTest::gtest_main)

# Добавляем тесты в CTest
include(GoogleTest)
gtest_discover_tests(robot_tests)