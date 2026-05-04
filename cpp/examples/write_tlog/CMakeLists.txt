cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(write_tlog)

add_executable(write_tlog
    write_tlog.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(write_tlog
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(write_tlog PRIVATE -Wall -Wextra)
else()
    add_compile_options(write_tlog PRIVATE -W2)
endif()
