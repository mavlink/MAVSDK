cmake_minimum_required(VERSION 2.8.12)

project(battery)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror ")
    # This warning is triggered by the MAVLink includes.
    add_definitions("-Wno-address-of-packed-member")
else()
    add_definitions("-std=c++11 -WX -W2")
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
