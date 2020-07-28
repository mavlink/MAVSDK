cmake_minimum_required(VERSION 2.8.12)

project(manual_control)

if(MSVC)
    add_definitions("-std=c++11 -WX -W2")
else()
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
endif()

find_package(MAVSDK REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

add_executable(manual_control
    manual_control.cpp
    joystick.cpp
)
if (NOT SDL2_LIBRARIES)
    set(SDL2_LIBRARIES SDL2::SDL2)
endif()

target_link_libraries(manual_control
    MAVSDK::mavsdk_action
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_manual_control
    MAVSDK::mavsdk
    ${SDL2_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
