cmake_minimum_required(VERSION 2.8.1)

project(gimbal_manager_test)

if (NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra") # -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(gimbal_manager_test
    gimbal_manager_test.cpp
)

target_link_libraries(gimbal_manager_test
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
)

