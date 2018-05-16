cmake_minimum_required(VERSION 2.8.12)

project(fly_qgc_mission)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
endif()

add_executable(fly_qgc_mission
    fly_qgc_mission.cpp
)

target_link_libraries(fly_qgc_mission
    dronecore
    dronecore_action
    dronecore_mission
    json11 # Needed by dronecore_mission for QGC mission plan import
    dronecore_telemetry
)
