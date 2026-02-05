cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(wind)

add_executable(wind
    wind.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(wind
    MAVSDK::mavsdk
)

