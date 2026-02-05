cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(follow_me)

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

find_package(MAVSDK REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(follow_me
    MAVSDK::mavsdk
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(follow_me PRIVATE -Wall -Wextra)
else()
    add_compile_options(follow_me PRIVATE -W2)
endif()
