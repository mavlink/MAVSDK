cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_direct)

find_package(MAVSDK REQUIRED)

add_executable(subscribe_gps
    subscribe_gps.cpp
)

target_link_libraries(subscribe_gps
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    target_compile_options(subscribe_gps PRIVATE -Wall -Wextra)
else()
    target_compile_options(subscribe_gps PRIVATE -W2)
endif()

add_executable(mavlink_stats
    mavlink_stats.cpp
)

target_link_libraries(mavlink_stats
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    target_compile_options(mavlink_stats PRIVATE -Wall -Wextra)
else()
    target_compile_options(mavlink_stats PRIVATE -W2)
endif()
