cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_direct_sender)

find_package(MAVSDK REQUIRED)

add_executable(sender
    sender.cpp
)

target_link_libraries(sender
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    target_compile_options(sender PRIVATE -Wall -Wextra)
else()
    target_compile_options(sender PRIVATE -W2)
endif()

