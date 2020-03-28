cmake_minimum_required(VERSION 3.5.1)

project(gimbal_device_test)

if (NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(gimbal_device_test
    gimbal_device_test.cpp
    )

target_link_libraries(gimbal_device_test
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
)
