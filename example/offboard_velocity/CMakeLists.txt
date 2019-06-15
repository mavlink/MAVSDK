cmake_minimum_required(VERSION 2.8.12)

project(offboard)

if(MINGW)
    add_definitions("-D_USE_MATH_DEFINES") # For M_PI
endif()

if(MSVC)
    add_definitions("-std=c++11 -WX -W2")
    add_definitions("-D_USE_MATH_DEFINES") # For M_PI
else()
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(offboard
    offboard_velocity.cpp
)

target_link_libraries(offboard
    DronecodeSDK::mavsdk_action
    DronecodeSDK::mavsdk_offboard
    DronecodeSDK::mavsdk_telemetry
    DronecodeSDK::mavsdk
)
