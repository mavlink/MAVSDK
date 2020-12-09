cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0025 NEW)

project(mavsdk)

include(cmake/ccache.cmake)
include(cmake/compiler_flags.cmake)

option(BUILD_TESTS "Build tests" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(tinyxml2 REQUIRED)

if(BUILD_TESTS AND (IOS OR ANDROID))
    message(STATUS "Building for iOS or Android: forcing BUILD_TESTS to FALSE...")
    set(BUILD_TESTS OFF)
endif()

add_subdirectory(core)
add_subdirectory(plugins)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/gtest EXCLUDE_FROM_ALL)

    add_subdirectory(integration_tests)

    include(cmake/unit_tests.cmake)
endif()

if (BUILD_MAVSDK_SERVER)
    message(STATUS "Building mavsdk server")
    add_subdirectory(mavsdk_server)
else()
    message(STATUS "BUILD_MAVSDK_SERVER not set: not building grpc mavsdk_server")
endif()

if (DROP_DEBUG EQUAL 1)
    add_definitions(-DDROP_DEBUG=${DROP_DEBUG})

    add_executable(drop_debug
        debug_helpers/drop_debug_main.cpp
    )

    target_link_libraries(drop_debug
        mavsdk
    )
endif()
