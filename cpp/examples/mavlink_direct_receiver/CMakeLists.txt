cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_direct_receiver)

find_package(MAVSDK REQUIRED)

add_executable(receiver
    receiver.cpp
)

target_link_libraries(receiver
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    target_compile_options(receiver PRIVATE -Wall -Wextra)
else()
    target_compile_options(receiver PRIVATE -W2)
endif()

