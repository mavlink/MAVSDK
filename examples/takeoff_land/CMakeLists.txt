cmake_minimum_required(VERSION 2.8.12)

project(takeoff_and_land)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

# for meson:
#find_library(MAVSDK NAMES libmavsdk)
#find_library(MAVSDK_ACTION NAMES libmavsdk_action)
#find_library(MAVSDK_TELEMETRY NAMES libmavsdk_telemetry)

add_executable(takeoff_and_land
    takeoff_and_land.cpp
)

target_link_libraries(takeoff_and_land
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
    # for meson:
    #MAVSDK
    #MAVSDK_ACTION
    #MAVSDK_TELEMETRY
)
