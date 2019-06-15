cmake_minimum_required(VERSION 2.8.12)

project(fly_qgc_mission)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(fly_qgc_mission
    fly_qgc_mission.cpp
)

target_link_libraries(fly_qgc_mission
    DronecodeSDK::mavsdk_action
    DronecodeSDK::mavsdk_mission
    DronecodeSDK::mavsdk_telemetry
    DronecodeSDK::mavsdk
)
