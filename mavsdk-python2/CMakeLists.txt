cmake_minimum_required(VERSION 3.10)
project(example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(pybind11)
pybind11_add_module(mavsdk2
    mavsdk-python.cpp
    plugin-action.cpp
    plugin-calibration.cpp
    plugin-camera.cpp
    plugin-failure.cpp
    plugin-follow_me.cpp
    plugin-ftp.cpp
    plugin-geofence.cpp
    plugin-gimbal.cpp
    plugin-info.cpp
    plugin-log_files.cpp
    plugin-manual_control.cpp
    plugin-mission.cpp
    plugin-mission_raw.cpp
    plugin-mocap.cpp
    plugin-offboard.cpp
    plugin-param.cpp
    plugin-shell.cpp
    plugin-telemetry.cpp
    plugin-tune.cpp)

find_package(MAVSDK)

target_link_libraries(mavsdk2
    PRIVATE pybind11::module
    MAVSDK::mavsdk
)
