cmake_minimum_required(VERSION 3.1)

set(CMAKE_INSTALL_PREFIX "../install" CACHE PATH "default cache path")
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_DIR}/third_party)

project(dronecore)

include(cmake/compiler_flags.cmake)
include(cmake/zlib.cmake)
include(cmake/curl.cmake)

set(UNIT_TEST_SOURCE_FILES "")

# We support install in order to use the header and library files in
# other applications.
if(ANDROID)
    set(lib_path "lib/android/${ANDROID_ABI}")
elseif(IOS)
    set(lib_path "lib/ios")
else()
    set(lib_path "lib")
endif()

# We want to use consistent install directories across libraries.
set(dronecore_install_include_dir "include/dronecore")
set(dronecore_install_lib_dir ${lib_path})

add_subdirectory(core)
add_subdirectory(plugins)

# Add external plugins.
if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
    add_subdirectory(${EXTERNAL_DIR}/plugins
        ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/plugins)
    include_directories(${EXTERNAL_DIR})
endif()

if (CMAKE_BUILD_DRONECORESERVER)
    message(STATUS "Building dronecore server")
    add_subdirectory(grpc/server)
else()
    message(STATUS "BUILD_DRONECORESERVER not set: not building dronecore server")
endif()

if(NOT IOS AND NOT ANDROID)
    enable_testing()
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/gtest EXCLUDE_FROM_ALL)

    # Build integration tests
    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    include(cmake/unit_tests.cmake)
endif()

if (DROP_DEBUG EQUAL 1)
    add_definitions(-DDROP_DEBUG=${DROP_DEBUG})

    add_executable(drop_debug
        debug_helpers/drop_debug_main.cpp
    )

    target_link_libraries(drop_debug
        dronecore
    )
endif()
