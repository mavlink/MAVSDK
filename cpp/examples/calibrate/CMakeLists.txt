cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(calibrate)

add_executable(calibrate
    calibrate.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(calibrate
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(calibrate PRIVATE -Wall -Wextra)
else()
    add_compile_options(calibrate PRIVATE -W2)
endif()
