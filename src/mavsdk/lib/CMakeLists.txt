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
    request_message.cpp
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

add_subdirectory(plugins)
