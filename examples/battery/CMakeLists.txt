cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(battery)

if(NOT MSVC)
    add_definitions("-Wall -Wextra ")
    # This warning is triggered by the MAVLink includes.
    add_definitions("-Wno-address-of-packed-member")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(battery
    battery.cpp
)

target_link_libraries(battery
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
)
