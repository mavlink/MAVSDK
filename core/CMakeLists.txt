find_package(Threads REQUIRED)

include_directories(
    ${DRONECORE_ZLIB_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/mavlink/include
)

if(IOS OR MSVC OR APPLE)
    set(LIBRARY_TYPE "STATIC")
else()
    set(LIBRARY_TYPE "SHARED")
endif()

add_library(dronecore ${LIBRARY_TYPE}
    call_every_handler.cpp
    connection.cpp
    curl_wrapper.cpp
    system.cpp
    mavlink_system.cpp
    dronecore.cpp
    dronecore_impl.cpp
    global_include.cpp
    http_loader.cpp
    mavlink_parameters.cpp
    mavlink_commands.cpp
    mavlink_channels.cpp
    mavlink_receiver.cpp
    plugin_base.cpp
    plugin_impl_base.cpp
    serial_connection.cpp
    tcp_connection.cpp
    timeout_handler.cpp
    udp_connection.cpp
    log.cpp
    cli_arg.cpp
    thread_pool.cpp
)

target_link_libraries(dronecore
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARY}
    ${TINYXML2_LIBRARY}
    ${DRONECORE_ZLIB_LIBRARIES}
)

if (IOS)
    target_link_libraries(dronecore
        "-framework Foundation"
        "-framework Security"
    )
endif()

# Link to Windows networking lib.
if (MSVC)
    target_link_libraries(dronecore
        ws2_32
    )
endif()

if(ANDROID)
    target_link_libraries(dronecore
        log
    )
endif()

set_target_properties(dronecore
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecore
    # TODO: not sure if this is needed
    #EXPORT dronecore-targets
    DESTINATION ${lib_path}
)

install(FILES
    connection_result.h
    system.h
    dronecore.h
    plugin_base.h
    ${plugin_header_paths}
    DESTINATION "include/dronecore"
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/core/global_include_test.cpp
    ${CMAKE_SOURCE_DIR}/core/mavlink_channels_test.cpp
    ${CMAKE_SOURCE_DIR}/core/unittests_main.cpp
    # TODO: add this again
    #${CMAKE_SOURCE_DIR}/core/http_loader_test.cpp
    ${CMAKE_SOURCE_DIR}/core/timeout_handler_test.cpp
    ${CMAKE_SOURCE_DIR}/core/call_every_handler_test.cpp
    ${CMAKE_SOURCE_DIR}/core/curl_test.cpp
    ${CMAKE_SOURCE_DIR}/core/any_test.cpp
    ${CMAKE_SOURCE_DIR}/core/cli_arg_test.cpp
    ${CMAKE_SOURCE_DIR}/core/locked_queue_test.cpp
    ${CMAKE_SOURCE_DIR}/core/thread_pool_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
