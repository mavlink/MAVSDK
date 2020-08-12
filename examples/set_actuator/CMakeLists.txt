cmake_minimum_required(VERSION 2.8.12)

project(set_actuator)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror ")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(set_actuator
    set_actuator.cpp
)

target_link_libraries(set_actuator
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
)
