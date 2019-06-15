cmake_minimum_required(VERSION 2.8.12)

project(fly_mission)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(fly_mission
    fly_mission.cpp
)

target_link_libraries(fly_mission
    DronecodeSDK::mavsdk_action
    DronecodeSDK::mavsdk_mission
    DronecodeSDK::mavsdk_telemetry
    DronecodeSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)
