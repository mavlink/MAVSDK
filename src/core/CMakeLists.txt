find_package(Threads REQUIRED)

include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

configure_file(version.h.in version.h)

add_library(mavsdk
    call_every_handler.cpp
    connection.cpp
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
    plugin_impl_base.cpp
    serial_connection.cpp
    tcp_connection.cpp
    timeout_handler.cpp
    udp_connection.cpp
    log.cpp
    cli_arg.cpp
    thread_pool.cpp
    geometry.cpp
    timesync.cpp
)

target_link_libraries(mavsdk
    PRIVATE
    CURL::libcurl
    ${CMAKE_THREAD_LIBS_INIT}
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
    system.h
    mavsdk.h
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
    ${PROJECT_SOURCE_DIR}/core/any_test.cpp
    ${PROJECT_SOURCE_DIR}/core/cli_arg_test.cpp
    ${PROJECT_SOURCE_DIR}/core/locked_queue_test.cpp
    ${PROJECT_SOURCE_DIR}/core/thread_pool_test.cpp
    ${PROJECT_SOURCE_DIR}/core/mavsdk_test.cpp
    ${PROJECT_SOURCE_DIR}/core/mavlink_mission_transfer_test.cpp
    ${PROJECT_SOURCE_DIR}/core/geometry_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
