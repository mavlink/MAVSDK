cmake_minimum_required(VERSION 3.5.1)

project(tune)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(tune
    tune.cpp
)

target_link_libraries(tune
    MAVSDK::mavsdk_tune
    MAVSDK::mavsdk
)
