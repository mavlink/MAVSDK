cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(log_streaming)

add_executable(log_streaming
    log_streaming.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(log_streaming
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(log_streaming PRIVATE -Wall -Wextra)
else()
    add_compile_options(log_streaming PRIVATE -W2)
endif()
