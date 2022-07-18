cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mission_manager)

add_subdirectory(rounded_corners)

add_executable(mission_manager
    mission_manager.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(mission_manager
    MAVSDK::mavsdk
    rounded_corners
)

if(NOT MSVC)
    add_compile_options(mission_manager PRIVATE -Wall -Wextra)
else()
    add_compile_options(mission_manager PRIVATE -WX -W2)
endif()

enable_testing()
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()
