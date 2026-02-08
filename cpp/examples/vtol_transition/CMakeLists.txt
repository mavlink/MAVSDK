cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(vtol_transition)

add_executable(vtol_transition
    vtol_transition.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(vtol_transition
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(vtol_transition PRIVATE -Wall -Wextra)
else()
    add_compile_options(vtol_transition PRIVATE -W2)
endif()
