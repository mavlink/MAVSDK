cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(callback_executor)

add_executable(callback_executor
    callback_executor.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(callback_executor
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(callback_executor PRIVATE -Wall -Wextra)
else()
    add_compile_options(callback_executor PRIVATE -W2)
endif()
