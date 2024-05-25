cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(winch)

add_executable(winch
    winch.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(winch
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(winch PRIVATE -Wall -Wextra)
else()
    add_compile_options(winch PRIVATE -W2)
endif()
