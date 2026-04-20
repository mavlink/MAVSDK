cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tlog_recorder)

find_package(MAVSDK REQUIRED)

add_executable(tlog_recorder
    tlog_recorder.cpp
)

target_link_libraries(tlog_recorder
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    target_compile_options(tlog_recorder PRIVATE -Wall -Wextra)
else()
    target_compile_options(tlog_recorder PRIVATE -W2)
endif()
