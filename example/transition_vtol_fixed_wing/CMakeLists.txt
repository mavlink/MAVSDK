cmake_minimum_required(VERSION 2.8.12)

project(transition_vtol_fixed_wing)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
endif()

add_executable(transition_vtol_fixed_wing
    transition_vtol_fixed_wing.cpp
)

target_link_libraries(transition_vtol_fixed_wing
    dronecore
)
