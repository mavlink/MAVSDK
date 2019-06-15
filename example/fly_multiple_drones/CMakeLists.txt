cmake_minimum_required(VERSION 2.8.12)

project(fly_multiple_drones)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(fly_multiple_drones
    fly_multiple_drones.cpp
)

target_link_libraries(fly_multiple_drones
    DronecodeSDK::mavsdk
    DronecodeSDK::mavsdk_telemetry
    DronecodeSDK::mavsdk_mission
    DronecodeSDK::mavsdk_action
    ${CMAKE_THREAD_LIBS_INIT}
)
