cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fly_qgc_mission)

add_executable(move_mission_to_vehicle
        main.cpp
        )

find_package(MAVSDK REQUIRED)

target_link_libraries(move_mission_to_vehicle
        MAVSDK::mavsdk
        )

if(NOT MSVC)
    add_compile_options(fly_qgc_mission PRIVATE -Wall -Wextra)
else()
    add_compile_options(fly_qgc_mission PRIVATE -WX -W2)
endif()
