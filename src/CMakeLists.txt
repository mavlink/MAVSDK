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

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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

set_target_properties(mavsdk PROPERTIES
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

set_target_properties(mavsdk
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS mavsdk
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

add_subdirectory(mavsdk)

include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

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

    if(BUILD_STATIC_MAVSDK_SERVER AND ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
        target_link_libraries(mavsdk PRIVATE atomic)
    endif()
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
