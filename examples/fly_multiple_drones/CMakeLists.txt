cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fly_multiple_drones)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-Wall -Wextra")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(fly_multiple_drones
    fly_multiple_drones.cpp
)

target_link_libraries(fly_multiple_drones
    MAVSDK::mavsdk
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_mission_raw
    MAVSDK::mavsdk_action
    ${CMAKE_THREAD_LIBS_INIT}
)
