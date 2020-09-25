cmake_minimum_required(VERSION 2.8.12)

project(fly_multiple_drones)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(fly_multiple_drones
    fly_multiple_drones.cpp
)

target_link_libraries(fly_multiple_drones
    MAVSDK::mavsdk
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_mission
    MAVSDK::mavsdk_action
    ${CMAKE_THREAD_LIBS_INIT}
)
