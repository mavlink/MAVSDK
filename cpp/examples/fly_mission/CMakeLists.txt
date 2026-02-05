cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fly_mission)

add_executable(fly_mission
    fly_mission.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(fly_mission
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(fly_mission PRIVATE -Wall -Wextra)
else()
    add_compile_options(fly_mission PRIVATE -W2)
endif()
