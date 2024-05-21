cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(set_actuator)

add_executable(set_actuator
    set_actuator.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(set_actuator
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(set_actuator PRIVATE -Wall -Wextra)
else()
    add_compile_options(set_actuator PRIVATE -W2)
endif()
