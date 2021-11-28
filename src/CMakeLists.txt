cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0025 NEW)

project(mavsdk)

# ccache
option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE AND CCACHE_PROGRAM AND NOT DEFINED ENV{CCACHE_DISABLE})

    get_filename_component(ccache_real_path ${CCACHE_PROGRAM} REALPATH)
    get_filename_component(cxx_real_path ${CMAKE_CXX_COMPILER} REALPATH)
    get_filename_component(cxx_abs_path ${CMAKE_CXX_COMPILER} ABSOLUTE)

    if ("${ccache_real_path}" STREQUAL "${cxx_real_path}")
        message(STATUS "ccache enabled via symlink (${cxx_abs_path} -> ${cxx_real_path})")
    else()
        message(STATUS "ccache enabled (export CCACHE_DISABLE=1 to disable)")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()
endif()

option(BUILD_TESTS "Build tests" ON)
option(CMAKE_POSITION_INDEPENDENT_CODE "Position independent code" ON)

include(cmake/compiler_flags.cmake)

find_package(Threads REQUIRED)

if(NOT HUNTER_ENABLED)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
endif()

# This will noop if hunter is not enabled
hunter_add_package(jsoncpp)
hunter_add_package(tinyxml2)
hunter_add_package(CURL)

find_package(CURL REQUIRED)
find_package(tinyxml2 REQUIRED)

if(BUILD_TESTS AND (IOS OR ANDROID))
    message(STATUS "Building for iOS or Android: forcing BUILD_TESTS to FALSE...")
    set(BUILD_TESTS OFF)
endif()

add_library(mavsdk
    # source files are added in subdirectories.
)

target_link_libraries(mavsdk
    PRIVATE
    CURL::libcurl
    Threads::Threads
    #JsonCpp::jsoncpp
    tinyxml2::tinyxml2
)

if (IOS)
    target_link_libraries(mavsdk
        PUBLIC
        "-framework Foundation"
        "-framework Security"
    )
elseif (MSVC OR MINGW)
    # Link to Windows networking lib.
    target_link_libraries(mavsdk
        PRIVATE
        ws2_32
    )
elseif(ANDROID)
    target_link_libraries(mavsdk
        PRIVATE
        log
    )
endif()

set(mavsdk_install_include_dir "include/mavsdk")
set(mavsdk_install_lib_dir ${lib_path})

add_subdirectory(mavsdk)

if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
    add_subdirectory(${EXTERNAL_DIR}/plugins
        ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/plugins)
    include_directories(${EXTERNAL_DIR})
endif()

include(cmake/static_analyzers.cmake)

if(BUILD_TESTS AND false)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/gtest EXCLUDE_FROM_ALL)

    #add_subdirectory(integration_tests)

    list(APPEND UNIT_TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/mavsdk/global_include_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/mavlink_channels_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/unittests_main.cpp
        # TODO: add this again
        #${PROJECT_SOURCE_DIR}/mavsdk/http_loader_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/timeout_handler_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/call_every_handler_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/curl_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/cli_arg_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/locked_queue_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/safe_queue_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/mavsdk_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/mavlink_mission_transfer_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/mavlink_statustext_handler_test.cpp
        ${PROJECT_SOURCE_DIR}/mavsdk/geometry_test.cpp
    )
    set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)

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
