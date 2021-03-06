cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gimbal)

if(NOT MSVC)
    add_definitions("-Wall -Wextra")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(gimbal
    gimbal.cpp
)

target_link_libraries(gimbal
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_gimbal
    MAVSDK::mavsdk
)
