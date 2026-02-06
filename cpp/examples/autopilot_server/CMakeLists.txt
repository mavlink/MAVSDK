cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(autopilot_server)

add_executable(autopilot_server
    autopilot_server.cpp
)

find_package(MAVSDK REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(autopilot_server
    MAVSDK::mavsdk
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(autopilot_server PRIVATE -Wall -Wextra)
else()
    add_compile_options(autopilot_server PRIVATE -W2)
endif()
