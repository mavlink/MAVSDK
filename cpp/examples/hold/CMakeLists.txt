cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hold)

add_executable(hold
    hold.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(hold
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(hold PRIVATE -Wall -Wextra)
else()
    add_compile_options(hold PRIVATE -W2)
endif()
