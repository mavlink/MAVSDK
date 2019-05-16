cmake_minimum_required(VERSION 2.8.12)

project(geofence_inclusion)

if(MSVC)
    add_definitions("-std=c++11 -WX -W2")
else()
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(geofence_inclusion
    geofence_inclusion.cpp
)

target_link_libraries(geofence_inclusion
    DronecodeSDK::dronecode_sdk_action
    DronecodeSDK::dronecode_sdk_mission
    DronecodeSDK::dronecode_sdk_telemetry
    DronecodeSDK::dronecode_sdk_geofence
    DronecodeSDK::dronecode_sdk
)
