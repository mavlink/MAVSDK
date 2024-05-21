cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(distance_sensor)

add_executable(distance_sensor
    distance_sensor.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(distance_sensor
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(distance_sensor PRIVATE -Wall -Wextra)
else()
    add_compile_options(distance_sensor PRIVATE -W2)
endif()
