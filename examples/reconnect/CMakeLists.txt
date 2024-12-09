cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(reconnect)

add_executable(reconnect
    reconnect.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(reconnect
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(reconnect PRIVATE -Wall -Wextra)
else()
    add_compile_options(reconnect PRIVATE -W2)
endif()
