cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(manual_control)

if(MSVC)
    add_definitions("-WX -W2")
else()
    add_definitions("-Wall -Wextra")
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
