cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavshell)

add_executable(mavshell
    mavshell.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(mavshell
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(mavshell PRIVATE -Wall -Wextra)
else()
    add_compile_options(mavshell PRIVATE -W2)
endif()
