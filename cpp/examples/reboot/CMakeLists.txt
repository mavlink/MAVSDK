cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(reboot)

add_executable(reboot
    reboot.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(reboot
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(reboot PRIVATE -Wall -Wextra)
else()
    add_compile_options(reboot PRIVATE -W2)
endif()
