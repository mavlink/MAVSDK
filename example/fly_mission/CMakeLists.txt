cmake_minimum_required(VERSION 2.8.12)

project(fly_mission)

include_directories(
    ${CMAKE_SOURCE_DIR}/../../core
    ${CMAKE_SOURCE_DIR}/../../libs/include
    ${CMAKE_SOURCE_DIR}/../../libs/json11
)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
endif()

add_library(json11 ${CMAKE_SOURCE_DIR}/../../libs/json11/json11.cpp)
add_executable(fly_mission fly_mission.cpp )
target_link_libraries(fly_mission
    dronecore
    json11 # required for parsing QGC plan files
)
