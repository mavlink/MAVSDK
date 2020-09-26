cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(offboard)

if(MINGW OR MSVC)
    add_definitions("-D_USE_MATH_DEFINES") # For M_PI
endif()

if(MSVC)
    add_definitions("-WX -W2")
else()
    add_definitions("-Wall -Wextra")
endif()

find_package(MAVSDK REQUIRED)

add_executable(offboard
    offboard_velocity.cpp
)

target_link_libraries(offboard
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_offboard
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
)
