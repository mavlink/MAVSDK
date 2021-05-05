cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(set_actuator)

if(MSVC)
    add_definitions("-WX -W2")
else()
    add_definitions("-Wall -Wextra")
endif()

find_package(MAVSDK REQUIRED)

add_executable(set_actuator
    set_actuator.cpp
)

target_link_libraries(set_actuator
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
)
