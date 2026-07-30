cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gimbal_device)

add_executable(gimbal_device
    gimbal_device.cpp
)

find_package(MAVSDK REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(gimbal_device
    MAVSDK::mavsdk
    nlohmann_json::nlohmann_json
)

if(NOT MSVC)
    add_compile_options(gimbal_device PRIVATE -Wall -Wextra)
else()
    add_compile_options(gimbal_device PRIVATE -W2)
endif()
