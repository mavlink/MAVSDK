cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(set_gps_origin)

add_executable(set_gps_origin
    set_gps_origin.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(set_gps_origin
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(set_gps_origin PRIVATE -Wall -Wextra)
else()
    add_compile_options(set_gps_origin PRIVATE -W2)
endif()
