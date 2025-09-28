cmake_minimum_required(VERSION 3.22.1)
project(cmavsdk
    VERSION 3.10.2
    DESCRIPTION "C API wrapper for MAVSDK"
    LANGUAGES C CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(MAVSDK REQUIRED)

set(C_API_SOURCES
    mavsdk.cpp
    system.cpp
)

set(C_API_HEADERS
    mavsdk.h
    system.h
    mavsdk_export.h
)

add_library(cmavsdk)

target_sources(cmavsdk
    PRIVATE
        ${C_API_SOURCES}
        ${C_API_HEADERS}
)

target_include_directories(cmavsdk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(cmavsdk
    PRIVATE
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

set_target_properties(cmavsdk PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 3
    EXPORT_NAME CMAVSDK
)

install(TARGETS cmavsdk
    EXPORT cmavsdkTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    FILES
        ${C_API_HEADERS}
    DESTINATION include/cmavsdk
)

install(EXPORT cmavsdkTargets
    FILE cmavsdkConfig.cmake
    NAMESPACE CMAVSDK::
    DESTINATION lib/cmake/cmavsdk
)

