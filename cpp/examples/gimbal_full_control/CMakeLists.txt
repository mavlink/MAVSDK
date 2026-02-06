cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gimbal_full_control)

add_executable(gimbal_full_control
    gimbal_full_control.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(gimbal_full_control
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(gimbal PRIVATE -Wall -Wextra)
else()
    add_compile_options(gimbal PRIVATE -W2)
endif()
