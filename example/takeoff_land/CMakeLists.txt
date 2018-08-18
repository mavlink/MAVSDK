cmake_minimum_required(VERSION 2.8.12)

project(takeoff_and_land)

add_definitions(-std=c++11)

if(NOT MSVC)
    add_definitions(-Wall -Wextra -Werror)
else()
    add_definitions(-WX -W2)
endif()

# Line below required if /usr/local/include is not in your default includes
#include_directories(/usr/local/include)
# Line below required if /usr/local/lib is not in your default linker path
#link_directories(/usr/local/lib)

if (LOCAL_INSTALL)
    include_directories(${CMAKE_SOURCE_DIR}/../../build/default/install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../build/default/install/lib)
endif()

add_executable(takeoff_and_land
    takeoff_and_land.cpp
)

target_link_libraries(takeoff_and_land
    dronecode_sdk
    dronecode_sdk_telemetry
    dronecode_sdk_action
)

