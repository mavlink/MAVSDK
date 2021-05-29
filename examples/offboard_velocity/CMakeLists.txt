cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(offboard_velocity)

add_executable(offboard_velocity
    offboard_velocity.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(offboard_velocity
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_offboard
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(offboard_velocity PRIVATE -Wall -Wextra)
else()
    add_compile_options(offboard_velocity PRIVATE -WX -W2)
endif()

if(MINGW OR MSVC)
    # For M_PI
    add_target_definitions(offboard_velocity "-D_USE_MATH_DEFINES")
endif()
