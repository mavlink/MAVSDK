cmake_minimum_required(VERSION 2.8.12)

project(follow_me)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

target_link_libraries(follow_me
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_follow_me
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)
