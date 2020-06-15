cmake_minimum_required(VERSION 2.8.12)

project(node_test)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(node_test
    node_test.cpp
)

target_link_libraries(node_test
    #MAVSDK::mavsdk_telemetry
    #MAVSDK::mavsdk_action
    MAVSDK::mavsdk
)
