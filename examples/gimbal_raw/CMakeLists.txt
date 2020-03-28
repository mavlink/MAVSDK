cmake_minimum_required(VERSION 3.5.1)

project(gimbal_raw)

if (NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(gimbal_raw
    gimbal_raw.cpp
    )

target_link_libraries(gimbal_raw
    MAVSDK::mavsdk_gimbal_manager
    MAVSDK::mavsdk
)
