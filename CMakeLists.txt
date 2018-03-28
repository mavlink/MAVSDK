cmake_minimum_required(VERSION 3.1)

set(CMAKE_INSTALL_PREFIX "../install" CACHE PATH "default cache path")
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_DIR}/third_party)

cmake_policy(SET CMP0025 NEW)

project(dronecore)

option(BUILD_TESTS "Build tests" ON)

include(cmake/compiler_flags.cmake)
include(cmake/zlib.cmake)
include(cmake/curl.cmake)

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

set(dronecore_install_include_dir "include/dronecore")
set(dronecore_install_lib_dir ${lib_path})

# We need tinyxml2 for the camera definition parsing.
add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
link_directories(third_party/tinyxml2)
include_directories(SYSTEM third_party/tinyxml2)
set(TINYXML2_LIBRARY tinyxml2)

if(APPLE AND NOT IOS)
    # We install the tinyxml2 library manually for macOS and iOS.
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Need to remove that d again.
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2/libtinyxml2d.6.0.0.dylib
            DESTINATION ${lib_path}
            #RENAME libtinyxml2.dylib
        )
    else()
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2/libtinyxml2.6.0.0.dylib
            DESTINATION ${lib_path}
            #RENAME libtinyxml2.dylib
        )
    endif()
elseif(ANDROID)
    # We install the tinyxml2 library manually for Android.
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Need to remove that d again.
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2/libtinyxml2d.so
            DESTINATION ${lib_path}
            RENAME libtinyxml2.so
        )
    else()
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2/libtinyxml2.so
            DESTINATION ${lib_path}
        )
    endif()
endif()

add_subdirectory(core)
add_subdirectory(plugins)

if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
    add_subdirectory(${EXTERNAL_DIR}/plugins
        ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/plugins)
    include_directories(${EXTERNAL_DIR})
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/gtest EXCLUDE_FROM_ALL)

    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    include(cmake/unit_tests.cmake)
endif()

if (CMAKE_BUILD_BACKEND)
    message(STATUS "Building dronecore server")
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
        dronecore
    )
endif()
