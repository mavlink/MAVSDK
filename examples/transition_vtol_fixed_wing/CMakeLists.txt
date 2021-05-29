cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(transition_vtol_fixed_wing)

add_executable(transition_vtol_fixed_wing
    transition_vtol_fixed_wing.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(transition_vtol_fixed_wing
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(transition_vtol_fixed_wing PRIVATE -Wall -Wextra)
else()
    add_compile_options(transition_vtol_fixed_wing PRIVATE -WX -W2)
endif()
