cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(offboard)

add_executable(offboard
    offboard.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(offboard
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(offboard PRIVATE -Wall -Wextra)
else()
    add_compile_options(offboard PRIVATE -W2)
endif()

if(MINGW OR MSVC)
    # For M_PI
    add_target_definitions(offboard "-D_USE_MATH_DEFINES")
endif()
