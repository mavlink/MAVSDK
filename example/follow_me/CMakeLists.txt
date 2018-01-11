cmake_minimum_required(VERSION 2.8.12)

project(follow_me)

find_package(Boost 1.66 COMPONENTS REQUIRED system)
include_directories(${Boost_INCLUDE_DIR})

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
endif()

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

target_link_libraries(follow_me
    LINK_PUBLIC ${Boost_LIBRARIES}
    dronecore
    pthread
)
