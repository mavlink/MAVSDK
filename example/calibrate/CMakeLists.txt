cmake_minimum_required(VERSION 2.8.12)

project(calibrate)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(calibrate
    calibrate.cpp
)

target_link_libraries(calibrate
    MAVSDK::mavsdk_calibration
    MAVSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)

