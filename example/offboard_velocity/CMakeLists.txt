cmake_minimum_required(VERSION 2.8.12)

project(offboard)

if(MSVC OR MINGW)
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
    DronecodeSDK::dronecode_sdk
    DronecodeSDK::dronecode_sdk_action
    DronecodeSDK::dronecode_sdk_offboard
    DronecodeSDK::dronecode_sdk_telemetry
)
