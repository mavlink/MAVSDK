cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(start_stop_server)

add_executable(start_stop_server
    start_stop_server.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(start_stop_server
    MAVSDK::mavsdk_server
)

if(NOT MSVC)
    add_compile_options(start_stop_server PRIVATE -Wall -Wextra)
else()
    add_compile_options(start_stop_server PRIVATE -W2)
endif()
