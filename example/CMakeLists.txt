cmake_minimum_required(VERSION 2.8.12)

project(takeoff_and_land)

add_definitions("-std=c++11 -Wall -Wextra -Werror")

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# This finds thread libs on Linux, Mac, and Windows.
find_package(Threads REQUIRED)

include_directories(
    # Not needed if installed system-wide
    ../install/include
)

add_executable(takeoff_and_land
    takeoff_and_land.cpp
)

target_link_libraries(takeoff_and_land
    ${CMAKE_SOURCE_DIR}/../install/lib/libdronelink.a
    # If installed system-wide:
    # dronelink
    ${CMAKE_THREAD_LIBS_INIT}
)
