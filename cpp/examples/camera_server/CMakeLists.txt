cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(camera_server)

add_executable(camera_server
    camera_server.cpp
)

add_executable(camera_client
    camera_client.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(camera_server
    MAVSDK::mavsdk
)

target_link_libraries(camera_client
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(camera_server PRIVATE -Wall -Wextra)
else()
    add_compile_options(camera_server PRIVATE -W2)
endif()
