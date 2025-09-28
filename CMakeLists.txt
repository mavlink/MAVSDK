cmake_minimum_required(VERSION 3.22.1)
project(cmavsdk)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(MAVSDK REQUIRED)

add_library(cmavsdk)

target_sources(cmavsdk
    PRIVATE
    mavsdk.cpp
)

target_include_directories(cmavsdk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(cmavsdk
    MAVSDK::mavsdk
)

target_compile_definitions(cmavsdk
    PRIVATE
    CMAVSDK_BUILD
)

target_compile_options(cmavsdk
    PRIVATE
    -Wall
    -Wextra
    #-Werror
    -fvisibility=hidden
)

install(TARGETS cmavsdk
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    FILES
    mavsdk.h
    mavsdk_export.h
    DESTINATION include/cmavsdk
)

set_target_properties(cmavsdk PROPERTIES
    VERSION 3.10.2
    SOVERSION 3
)

