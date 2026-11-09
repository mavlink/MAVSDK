cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(timesync)

add_executable(timesync
    timesync.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(timesync
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(timesync PRIVATE -Wall -Wextra)
else()
    add_compile_options(timesync PRIVATE -W2)
endif()
