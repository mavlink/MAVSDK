cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(geofence_inclusion)

add_executable(geofence_inclusion
    geofence_inclusion.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(geofence_inclusion
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(geofence_inclusion PRIVATE -Wall -Wextra)
else()
    add_compile_options(geofence_inclusion PRIVATE -WX -W2)
endif()
