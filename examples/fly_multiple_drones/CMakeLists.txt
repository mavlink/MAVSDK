cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fly_multiple_drones)

add_executable(fly_multiple_drones
    fly_multiple_drones.cpp
)

find_package(Threads REQUIRED)
find_package(MAVSDK REQUIRED)

target_link_libraries(fly_multiple_drones
    MAVSDK::mavsdk
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(fly_multiple_drones PRIVATE -Wall -Wextra)
else()
    add_compile_options(fly_multiple_drones PRIVATE -W2)
endif()
