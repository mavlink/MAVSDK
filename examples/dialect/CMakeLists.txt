cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dialect)

if(NOT MSVC)
    add_definitions("-Wall -Wextra ")
    add_definitions("-Wno-address-of-packed-member")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(dialect_client
    dialect_client.cpp
)

target_link_libraries(dialect_client
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
)

add_executable(dialect_server
    dialect_server.cpp
)

target_link_libraries(dialect_server
    MAVSDK::mavsdk_mavlink_passthrough
    MAVSDK::mavsdk
)
