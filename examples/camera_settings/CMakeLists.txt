cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_settings)

if(NOT MSVC)
    add_definitions("-Wall -Wextra")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(camera_settings
    camera_settings.cpp
)

target_link_libraries(camera_settings
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_camera
    MAVSDK::mavsdk
)
