cmake_minimum_required(VERSION 3.14)
project(KettleProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Собираем исполняемый файл 'kettle'
add_executable(kettle
    main.cpp
    Kettle.cpp
    HeatingStrategy.cpp
    # Если у тебя есть NotificationManager.cpp — добавь его тоже
)

# Если у тебя есть отдельный файл NotificationManager.cpp:
# add_executable(kettle
#     main.cpp
#     Kettle.cpp
#     HeatingStrategy.cpp
#     NotificationManager.cpp
# )