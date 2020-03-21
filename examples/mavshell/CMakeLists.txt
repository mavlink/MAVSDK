cmake_minimum_required(VERSION 2.8.12)

project(mavshell)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(mavshell
    mavshell.cpp
)

target_link_libraries(mavshell
    MAVSDK::mavsdk_shell
    MAVSDK::mavsdk
)
