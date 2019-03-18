cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

#list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_DIR}/third_party)

cmake_policy(SET CMP0025 NEW)

project(dronecode_sdk)

# ccache
option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if (CCACHE AND CCACHE_PROGRAM AND NOT DEFINED ENV{CCACHE_DISABLE})

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

include(cmake/compiler_flags.cmake)
#include(cmake/curl.cmake)
#include(cmake/tinyxml2.cmake)
#include(cmake/zlib.cmake)

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

execute_process(
    COMMAND git describe --abbrev=8 --dirty --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

STRING(REGEX REPLACE v\([0-9]+.[0-9]+.[0-9]+.*$\) \\1 VERSION_STR "${VERSION_STR}")

message(STATUS "Version: ${VERSION_STR}")
add_definitions(-DDRONECODE_SDK_VERSION="${VERSION_STR}")

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
