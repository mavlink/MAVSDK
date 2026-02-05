cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(params)

add_executable(params
    params.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(params
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(params PRIVATE -Wall -Wextra)
else()
    add_compile_options(params PRIVATE -W2)
endif()
