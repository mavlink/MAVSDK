cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gimbal_device_tester)

add_executable(gimbal_device_tester
    gimbal_device_tester.cpp
)

find_package(MAVSDK REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(gimbal_device_tester
    MAVSDK::mavsdk
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(gimbal_device_tester PRIVATE -Wall -Wextra)
else()
    add_compile_options(gimbal_device_tester PRIVATE -W2)
endif()
