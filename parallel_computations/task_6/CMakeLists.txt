cmake_minimum_required(VERSION 3.10)
project(heat_solver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Boost
find_package(Boost COMPONENTS program_options REQUIRED)

# Флаги компиляции для pgc++ с OpenACC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc -Minfo=all")

# Опции для выбора целевого устройства
option(ACC_GPU "Target GPU (default)" ON)
if(ACC_GPU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc=gpu")
else()
    option(ACC_MULTICORE "Target multicore CPU" OFF)
    if(ACC_MULTICORE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc=multicore")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc=host")
    endif()
endif()

add_executable(heat_solver heat_distribution.cpp)
target_link_libraries(heat_solver Boost::program_options)

# Установка rpath для библиотек NVIDIA HPC SDK (если нужно)
set_target_properties(heat_solver PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    BUILD_WITH_INSTALL_RPATH FALSE
)