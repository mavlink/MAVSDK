cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_forwarding)

add_executable(mavlink_forwarding
    mavlink_forwarding.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(mavlink_forwarding
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(mavlink_forwarding PRIVATE -Wall -Wextra)
else()
    add_compile_options(mavlink_forwarding PRIVATE -W2)
endif()
