cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(manual_control)

add_executable(manual_control
    manual_control.cpp
    joystick.cpp
)

find_package(MAVSDK REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

# Workaround for Ubuntu 18.04 where SDL2::SDL2 isn't provided yet.
if (SDL2_FOUND AND NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 UNKNOWN IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${SDL2_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
    )
endif()


target_link_libraries(manual_control
    MAVSDK::mavsdk
    SDL2::SDL2
    Threads::Threads
)

if(NOT MSVC)
    add_compile_options(manual_control PRIVATE -Wall -Wextra)
else()
    add_compile_options(manual_control PRIVATE -W2)
endif()
