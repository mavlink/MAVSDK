cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(modes)

add_executable(modes
    modes.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(modes
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(modes PRIVATE -Wall -Wextra)
else()
    add_compile_options(modes PRIVATE -W2)
endif()
