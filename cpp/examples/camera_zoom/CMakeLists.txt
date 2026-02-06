cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_zoom)

add_executable(camera_zoom
    camera_zoom.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(camera_zoom
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera_zoom PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera_zoom PRIVATE -W2)
endif()
