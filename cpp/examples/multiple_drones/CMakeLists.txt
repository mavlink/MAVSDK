cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(multiple_drones)

add_executable(multiple_drones
    multiple_drones.cpp
)

find_package(MAVSDK REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(multiple_drones
    MAVSDK::mavsdk
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(multiple_drones PRIVATE -Wall -Wextra)
else()
    add_compile_options(multiple_drones PRIVATE -W2)
endif()
