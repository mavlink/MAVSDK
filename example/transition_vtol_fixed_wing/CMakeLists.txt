cmake_minimum_required(VERSION 2.8.12)

project(transition_vtol_fixed_wing)

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

# Not needed if DroneCore installed system-wide
include_directories(
    ${CMAKE_SOURCE_DIR}/../../install/include
)

add_executable(transition_vtol_fixed_wing
    transition_vtol_fixed_wing.cpp
)

# Not needed if DroneCore installed system-wide
if(MSVC)
    set(dronecore_lib "${CMAKE_SOURCE_DIR}/../../install/lib/dronecore.lib")
else()
    set(dronecore_lib "${CMAKE_SOURCE_DIR}/../../install/lib/libdronecore.so")
endif()

target_link_libraries(transition_vtol_fixed_wing
    ${dronecore_lib}  # Remove/comment out this line if DroneCore used locally
    # dronecore  # Uncomment/add this line if DroneCore installed system-wide
    ${CMAKE_THREAD_LIBS_INIT}
    ${platform_libs}
)
