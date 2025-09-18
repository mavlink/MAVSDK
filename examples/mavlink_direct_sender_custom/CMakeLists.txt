cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_direct_sender_custom)

find_package(MAVSDK REQUIRED)

add_executable(sender_custom
    sender_custom.cpp
)

target_link_libraries(sender_custom
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    target_compile_options(sender_custom PRIVATE -Wall -Wextra)
else()
    target_compile_options(sender_custom PRIVATE -W2)
endif()

