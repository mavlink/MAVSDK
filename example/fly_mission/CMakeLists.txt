cmake_minimum_required(VERSION 2.8.12)

project(fly_mission)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
else()
    add_definitions("-std=c++11 -WX -W2")
    set(platform_libs "Ws2_32.lib")
endif()

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# This finds thread libs on Linux, Mac, and Windows.
find_package(Threads REQUIRED)

include_directories(
    # Not needed if installed system-wide
    ${CMAKE_SOURCE_DIR}/../../install/include
)

add_executable(fly_mission
    fly_mission_dronecore.cpp
)

if(WINDOWS)
    set(dronecore_lib "${CMAKE_SOURCE_DIR}/../../install/lib/dronecore.lib")
else()
    set(dronecore_lib "${CMAKE_SOURCE_DIR}/../../install/lib/libdronecore.a")
endif()

target_link_libraries(fly_mission
    ${dronecore_lib}
    # If installed system-wide:
    # dronecore
    ${CMAKE_THREAD_LIBS_INIT}
    ${platform_libs}
)
