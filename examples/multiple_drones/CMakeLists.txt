cmake_minimum_required(VERSION 2.8.12)

project(multiple_drones)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(multiple_drones
    multiple_drones.cpp
)

target_link_libraries(multiple_drones
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)
