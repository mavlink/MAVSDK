cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(battery)

add_executable(battery
    battery.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(battery
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(battery PRIVATE -Wall -Wextra)
else()
    add_compile_options(battery PRIVATE -W2)
endif()
