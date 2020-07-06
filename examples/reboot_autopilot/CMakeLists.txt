cmake_minimum_required(VERSION 2.8.12)

project(reboot_autopilot)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(reboot_autopilot
    reboot_autopilot.cpp
)

target_link_libraries(reboot_autopilot
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk
)
