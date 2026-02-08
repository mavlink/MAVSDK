cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(component_metadata)

add_executable(component_metadata
    component_metadata.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(component_metadata
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(component_metadata PRIVATE -Wall -Wextra)
else()
    add_compile_options(component_metadata PRIVATE -W2)
endif()
