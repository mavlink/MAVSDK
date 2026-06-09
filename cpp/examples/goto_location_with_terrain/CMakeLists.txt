cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(goto_location_with_terrain)

add_executable(goto_location_with_terrain
    goto_location_with_terrain.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(goto_location_with_terrain
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(goto_location_with_terrain PRIVATE -Wall -Wextra)
else()
    add_compile_options(goto_location_with_terrain PRIVATE -W2)
endif()
