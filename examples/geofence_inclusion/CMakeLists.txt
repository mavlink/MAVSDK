cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(geofence_inclusion)

if(MSVC)
    add_definitions("-WX -W2")
else()
    add_definitions("-Wall -Wextra")
endif()

find_package(MAVSDK REQUIRED)

add_executable(geofence_inclusion
    geofence_inclusion.cpp
)

target_link_libraries(geofence_inclusion
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_mission
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_geofence
    MAVSDK::mavsdk
)
