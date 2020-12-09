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

configure_file(core/version.h.in core/version.h)

add_library(mavsdk
    core/call_every_handler.cpp
    core/connection.cpp
    core/connection_result.cpp
    core/curl_wrapper.cpp
    core/system.cpp
    core/system_impl.cpp
    core/mavsdk.cpp
    core/mavsdk_impl.cpp
    core/global_include.cpp
    core/http_loader.cpp
    core/mavlink_channels.cpp
    core/mavlink_commands.cpp
    core/mavlink_mission_transfer.cpp
    core/mavlink_parameters.cpp
    core/mavlink_receiver.cpp
    core/mavlink_statustext_handler.cpp
    core/mavlink_message_handler.cpp
    core/ping.cpp
    core/plugin_impl_base.cpp
    core/serial_connection.cpp
    core/tcp_connection.cpp
    core/timeout_handler.cpp
    core/udp_connection.cpp
    core/log.cpp
    core/cli_arg.cpp
    core/geometry.cpp
    core/timesync.cpp
)

target_link_libraries(mavsdk
    PRIVATE
    CURL::libcurl
    Threads::Threads
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

target_include_directories(mavsdk
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/core>
    )

install(TARGETS mavsdk
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    core/connection_result.h
    core/deprecated.h
    core/system.h
    core/mavsdk.h
    core/plugin_base.h
    core/geometry.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mavsdk"
)

add_subdirectory(plugins)

include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/gtest EXCLUDE_FROM_ALL)

    add_subdirectory(integration_tests)

    list(APPEND UNIT_TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/core/global_include_test.cpp
        ${PROJECT_SOURCE_DIR}/core/mavlink_channels_test.cpp
        ${PROJECT_SOURCE_DIR}/core/unittests_main.cpp
        # TODO: add this again
        #${PROJECT_SOURCE_DIR}/core/http_loader_test.cpp
        ${PROJECT_SOURCE_DIR}/core/timeout_handler_test.cpp
        ${PROJECT_SOURCE_DIR}/core/call_every_handler_test.cpp
        ${PROJECT_SOURCE_DIR}/core/curl_test.cpp
        ${PROJECT_SOURCE_DIR}/core/cli_arg_test.cpp
        ${PROJECT_SOURCE_DIR}/core/locked_queue_test.cpp
        ${PROJECT_SOURCE_DIR}/core/safe_queue_test.cpp
        ${PROJECT_SOURCE_DIR}/core/mavsdk_test.cpp
        ${PROJECT_SOURCE_DIR}/core/mavlink_mission_transfer_test.cpp
        ${PROJECT_SOURCE_DIR}/core/mavlink_statustext_handler_test.cpp
        ${PROJECT_SOURCE_DIR}/core/geometry_test.cpp
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
