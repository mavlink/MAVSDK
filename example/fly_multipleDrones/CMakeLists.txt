
cmake_minimum_required(VERSION 2.8.12)

project(fly_multipleDrones)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(fly_multipleDrones
    fly_multipleDrones.cpp
)

target_link_libraries(fly_multipleDrones
    dronecode_sdk
    dronecode_sdk_telemetry
    dronecode_sdk_mission
    dronecode_sdk_action
    ${CMAKE_THREAD_LIBS_INIT}
)

