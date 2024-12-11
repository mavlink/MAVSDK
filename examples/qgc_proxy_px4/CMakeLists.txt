cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(qgc_proxy_px4)

add_executable(qgc_proxy_px4
    qgc_proxy_px4.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(qgc_proxy_px4
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(qgc_proxy_px4 PRIVATE -Wall -Wextra)
else()
    add_compile_options(qgc_proxy_px4 PRIVATE -W2)
endif()
