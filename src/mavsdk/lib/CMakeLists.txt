configure_file(version.h.in version.h)

target_sources(mavsdk
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/call_every_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_result.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curl_wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/system_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavsdk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavsdk_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/global_include.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/http_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_channels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_commands.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_mission_transfer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_parameters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_receiver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_statustext_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mavlink_message_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_impl_base.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serial_connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tcp_connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timeout_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/udp_connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cli_arg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/timesync.cpp
)

add_subdirectory(plugins)
