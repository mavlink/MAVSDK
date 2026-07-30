cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gimbal_manager)

add_executable(gimbal_manager
    gimbal_manager.cpp
)

find_package(MAVSDK REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(gimbal_manager
    MAVSDK::mavsdk
    nlohmann_json::nlohmann_json
)

if(NOT MSVC)
    add_compile_options(gimbal_manager PRIVATE -Wall -Wextra)
else()
    add_compile_options(gimbal_manager PRIVATE -W2)
endif()
