cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_proxy)

add_executable(mavlink_proxy
    mavlink_proxy.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(mavlink_proxy
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(mavlink_proxy PRIVATE -Wall -Wextra)
else()
    add_compile_options(mavlink_proxy PRIVATE -W2)
endif()
