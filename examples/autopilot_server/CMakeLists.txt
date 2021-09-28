cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(autopilot_server)

add_executable(autopilot_server
    autopilot_server.cpp
)

find_package(MAVSDK REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(autopilot_server
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_telemetry_server
    MAVSDK::mavsdk_mission_raw_server
    MAVSDK::mavsdk_mission
    MAVSDK::mavsdk_param_server
    MAVSDK::mavsdk_param
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_action_server
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(autopilot_server PRIVATE -Wall -Wextra)
else()
    add_compile_options(autopilot_server PRIVATE -WX -W2)
endif()
