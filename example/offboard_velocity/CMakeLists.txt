cmake_minimum_required(VERSION 2.8.12)

project(offboard)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
endif()

add_executable(offboard
    offboard_velocity.cpp
)

target_link_libraries(offboard
    dronecore
)
