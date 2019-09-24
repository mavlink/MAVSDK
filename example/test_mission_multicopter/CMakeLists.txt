cmake_minimum_required(VERSION 3.5.1)

project(test_mission_multicopter)

add_definitions("-std=c++11 -Wall -Wextra -Werror")

find_package(MAVSDK REQUIRED)

add_executable(test_mission_multicopter
    autopilot_tester.cpp
    test_mission_multicopter.cpp
)

target_link_libraries(test_mission_multicopter
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_path_checker
    MAVSDK::mavsdk
    gtest
    gtest_main
)
