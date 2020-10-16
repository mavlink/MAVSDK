cmake_minimum_required(VERSION 2.8.12)

project(gimbal_device_tester)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra ")
    # This warning is triggered by the MAVLink includes.
    add_definitions("-Wno-address-of-packed-member")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)
find_package(Threads REQUIRED)

add_executable(gimbal_device_tester
    gimbal_device_tester.cpp
)

target_link_libraries(gimbal_device_tester
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)
