cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(events)

add_executable(events
    events.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(events
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(events PRIVATE -Wall -Wextra)
else()
    add_compile_options(events PRIVATE -W2)
endif()
