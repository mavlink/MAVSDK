cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(set_relay)

add_executable(set_relay
    set_relay.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(set_relay
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(set_relay PRIVATE -Wall -Wextra)
else()
    add_compile_options(set_relay PRIVATE -W2)
endif()
