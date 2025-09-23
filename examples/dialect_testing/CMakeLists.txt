cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dialect_testing)

find_package(MAVSDK REQUIRED)

add_executable(sender
    sender.cpp
)

target_link_libraries(sender
    PRIVATE
    MAVSDK::mavsdk
)

add_executable(receiver
    receiver.cpp
)

target_link_libraries(receiver
    PRIVATE
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(dialect_testing PRIVATE -Wall -Wextra)
else()
    add_compile_options(dialect_testing PRIVATE -W2)
endif()
