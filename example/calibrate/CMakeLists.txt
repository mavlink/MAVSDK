cmake_minimum_required(VERSION 2.8.12)

project(calibrate)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(DronecodeSDK REQUIRED)

add_executable(calibrate
    calibrate.cpp
)

target_link_libraries(calibrate
    DronecodeSDK::dronecode_sdk_calibration
    DronecodeSDK::dronecode_sdk
    ${CMAKE_THREAD_LIBS_INIT}
)

