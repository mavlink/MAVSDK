cmake_minimum_required(VERSION 2.8.12)

project(manual_control)

if(MSVC)
    add_definitions("-std=c++11 -WX -W2")
else()
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
endif()

find_package(MAVSDK REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(manual_control
    manual_control.cpp
    joystick.cpp
)

target_link_libraries(manual_control
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
    SDL2::SDL2
)
