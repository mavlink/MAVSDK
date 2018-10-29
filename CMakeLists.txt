cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_DIR}/third_party)

cmake_policy(SET CMP0025 NEW)

project(dronecode_sdk)

option(BUILD_TESTS "Build tests" ON)

include(cmake/compiler_flags.cmake)
include(cmake/curl.cmake)
include(cmake/tinyxml2.cmake)
include(cmake/zlib.cmake)

if(NOT MSVC)
    cmake_policy(SET CMP0058 NEW)
    include(third_party/cpp_rsc/cmake/modules/cpp_resource.cmake)
    if(NOT ANDROID AND NOT IOS)
        # If not cross-compiling, we can build cpp_rsc.
        add_subdirectory(third_party/cpp_rsc/src EXCLUDE_FROM_ALL)
    else()
        # For cross-compilation, re-use the binary from default.
        set_property(GLOBAL PROPERTY CPPRSC_CMD ${CMAKE_CURRENT_BINARY_DIR}/../default/third_party/cpp_rsc/src/cpp_rsc)
    endif()
endif()

if(BUILD_TESTS AND (IOS OR ANDROID))
    message(STATUS "Building for iOS or Android: forcing BUILD_TESTS to FALSE...")
    set(BUILD_TESTS OFF)
endif()

if(ANDROID)
    set(lib_path "lib/android/${ANDROID_ABI}")
elseif(IOS)
    set(lib_path "lib/ios")
else()
    set(lib_path "lib")
endif()

set(dronecode_sdk_install_include_dir "include/dronecode_sdk")
set(dronecode_sdk_install_lib_dir ${lib_path})

file (STRINGS "${PROJECT_SOURCE_DIR}/version" VERSION_STR)
message(STATUS "Read version: ${VERSION_STR}")
add_definitions(-DDRONECODE_SDK_VERSION="${VERSION_STR}")

add_custom_target(version
    COMMAND :
    DEPENDS ${PROJECT_SOURCE_DIR}/version)

add_subdirectory(core)
add_subdirectory(plugins)

if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
    add_subdirectory(${EXTERNAL_DIR}/plugins
        ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/plugins)
    include_directories(${EXTERNAL_DIR})
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/gtest EXCLUDE_FROM_ALL)

    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    include(cmake/unit_tests.cmake)
endif()

if (CMAKE_BUILD_BACKEND)
    message(STATUS "Building dronecode_sdk server")
    add_subdirectory(backend)
else()
    message(STATUS "BUILD_BACKEND not set: not building grpc backend")
endif()

if (DROP_DEBUG EQUAL 1)
    add_definitions(-DDROP_DEBUG=${DROP_DEBUG})

    add_executable(drop_debug
        debug_helpers/drop_debug_main.cpp
    )

    target_link_libraries(drop_debug
        dronecode_sdk
    )
endif()
