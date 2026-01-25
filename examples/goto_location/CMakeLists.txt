cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(goto_location)

add_executable(goto_location
    goto_location.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(goto_location
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(goto_location PRIVATE -Wall -Wextra)
else()
    add_compile_options(goto_location PRIVATE -W2)
endif()
