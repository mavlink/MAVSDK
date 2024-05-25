cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(system_info)

add_executable(system_info
    system_info.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(system_info
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(system_info PRIVATE -Wall -Wextra)
else()
    add_compile_options(system_info PRIVATE -W2)
endif()
