set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

configure_file(version.h.in version.h)

add_library(mavsdk
    call_every_handler.cpp
    connection.cpp
    connection_result.cpp
    curl_wrapper.cpp
    system.cpp
    system_impl.cpp
    mavsdk.cpp
    mavsdk_impl.cpp
    global_include.cpp
    http_loader.cpp
    mavlink_channels.cpp
    mavlink_commands.cpp
    mavlink_mission_transfer.cpp
    mavlink_parameters.cpp
    mavlink_receiver.cpp
    mavlink_statustext_handler.cpp
    mavlink_message_handler.cpp
    ping.cpp
    plugin_impl_base.cpp
    serial_connection.cpp
    tcp_connection.cpp
    timeout_handler.cpp
    udp_connection.cpp
    log.cpp
    cli_arg.cpp
    geometry.cpp
    timesync.cpp
)

target_link_libraries(mavsdk
    PRIVATE
    CURL::libcurl
    Threads::Threads
)
set_target_properties(mavsdk PROPERTIES
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

if (IOS)
    target_link_libraries(mavsdk
        PUBLIC
        "-framework Foundation"
        "-framework Security"
    )
endif()

# Link to Windows networking lib.
if (MSVC OR MINGW)
    target_link_libraries(mavsdk
        PRIVATE
        ws2_32
    )
endif()

if(ANDROID)
    target_link_libraries(mavsdk
        PRIVATE
        log
    )
endif()

if( (BUILD_STATIC_MAVSDK_SERVER AND ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
    OR (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "(armv6|armv7)") )
    target_link_libraries(mavsdk PRIVATE atomic)
endif()

set_target_properties(mavsdk
    PROPERTIES COMPILE_FLAGS ${warnings}
    )

target_include_directories(mavsdk
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS mavsdk
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    connection_result.h
    deprecated.h
    system.h
    mavsdk.h
    log_callback.h
    plugin_base.h
    geometry.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mavsdk"
)

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
    ${PROJECT_SOURCE_DIR}/core/ringbuffer_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
