cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera)

add_executable(camera
    camera.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(camera
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera PRIVATE -W2)
endif()
