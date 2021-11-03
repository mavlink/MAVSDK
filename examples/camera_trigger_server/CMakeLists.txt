cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_trigger_server)

add_executable(camera_trigger_server
    camera_trigger_server.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(camera_trigger_server
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_camera_trigger_server
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera_trigger_server PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera_trigger_server PRIVATE -WX -W2)
endif()
