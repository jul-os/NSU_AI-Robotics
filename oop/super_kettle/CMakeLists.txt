cmake_minimum_required(VERSION 3.14)
project(KettleProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Собираем исполняемый файл 'kettle'
add_executable(kettle
    main.cpp
    Kettle.cpp
    HeatingStrategy.cpp
)
