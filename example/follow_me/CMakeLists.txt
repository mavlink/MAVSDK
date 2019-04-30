cmake_minimum_required(VERSION 2.8.12)

project(follow_me)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

target_link_libraries(follow_me
    DronecodeSDK::dronecode_sdk_action
    DronecodeSDK::dronecode_sdk_follow_me
    DronecodeSDK::dronecode_sdk_telemetry
    DronecodeSDK::dronecode_sdk
    ${CMAKE_THREAD_LIBS_INIT}
)
