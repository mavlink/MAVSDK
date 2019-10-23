cmake_minimum_required(VERSION 2.8.12)

project(geofence_inclusion)

if(MSVC)
    add_definitions("-std=c++11 -WX -W2")
else()
    add_definitions("-std=c++11 -Wall -Wextra")
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
