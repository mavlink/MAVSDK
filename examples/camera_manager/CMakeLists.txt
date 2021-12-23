cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_manager)

add_executable(camera_manager
    camera_manager.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(camera_manager
    MAVSDK::mavsdk_ftp
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk_param_server
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera_manager PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera_manager PRIVATE -WX -W2)
endif()
