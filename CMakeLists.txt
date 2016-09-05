cmake_minimum_required(VERSION 2.8.12)

project(dronelink)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Weffc++ -Werror")

add_library(dronelink STATIC
    src/connection.cpp
    src/device.cpp
    src/device_impl.cpp
    src/info.cpp
    src/info_impl.cpp
    src/telemetry.cpp
    src/telemetry_impl.cpp
    src/dronelink.cpp
    src/dronelink_impl.cpp
    src/error_handling.cpp
    src/mavlink_channels.cpp
    src/mavlink_receiver.cpp
    src/serial_connection.cpp
    src/tcp_connection.cpp
    src/udp_connection.cpp
    src/math_conversions.cpp
)

include_directories(
    include
    libs/include
)

add_subdirectory(tests)
