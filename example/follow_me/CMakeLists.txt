cmake_minimum_required(VERSION 2.8.12)

project(follow_me)

if (MSVC)
    find_package(Boost 1.64 COMPONENTS system date_time REQUIRED )
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
    add_definitions("-std=c++11 -WX -W2")
else()
    find_package(Boost 1.58 COMPONENTS system REQUIRED )
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
endif()

include_directories(${Boost_INCLUDE_DIR})

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

target_link_libraries(follow_me
    LINK_PUBLIC ${Boost_LIBRARIES}
    dronecore
    dronecore_action
    dronecore_follow_me
    dronecore_telemetry
)
