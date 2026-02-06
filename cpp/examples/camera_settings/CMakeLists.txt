cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_settings)

add_executable(camera_settings
    camera_settings.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(camera_settings
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera_settings PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera_settings PRIVATE -W2)
endif()
