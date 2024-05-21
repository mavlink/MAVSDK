cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(log_callback)

add_executable(log_callback
    log_callback.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(log_callback
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(log_callback PRIVATE -Wall -Wextra)
else()
    add_compile_options(log_callback PRIVATE -W2)
endif()
