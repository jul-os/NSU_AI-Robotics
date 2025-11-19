cmake_minimum_required(VERSION 3.10)
project(SocialNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Network module sources
set(NETWORK_SOURCES
    network_module/user.cpp
    network_module/datetime.cpp
    network_module/social_network.cpp
)

set(NETWORK_HEADERS
    network_module/user.hpp
    network_module/post.hpp
    network_module/datetime.hpp
    network_module/social_network.hpp
)

# Simulation module sources
set(SIMULATION_SOURCES
    simulation_module/simulation.cpp
    simulation_module/main.cpp
    simulation_module/post_generator.cpp
)

set(SIMULATION_HEADERS
    simulation_module/simulation.hpp
    simulation_module/post_generator.hpp
)

# Include directories
include_directories(
    network_module
    simulation_module
)

# Создаем исполняемый файл
add_executable(social_network 
    ${NETWORK_SOURCES}
    ${NETWORK_HEADERS}
    ${SIMULATION_SOURCES}
    ${SIMULATION_HEADERS}
)

# Настройки компиляции
target_compile_features(social_network PRIVATE cxx_std_17)