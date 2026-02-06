cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(geofence)

add_executable(geofence
    geofence.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(geofence
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(geofence PRIVATE -Wall -Wextra)
else()
    add_compile_options(geofence PRIVATE -W2)
endif()
