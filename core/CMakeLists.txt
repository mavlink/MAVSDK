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

# We need tinyxml2 for the camera definition parsing.
if(APPLE)
    if(NOT IOS)
        # We install the tinyxml2 library manually for macOS and iOS.
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            # Need to remove that d again.
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2/libtinyxml2d.5.0.1.dylib
                DESTINATION ${lib_path}
                #RENAME libtinyxml2.dylib
            )
        else()
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/tinyxml2/libtinyxml2.5.0.1.dylib
                DESTINATION ${lib_path}
                #RENAME libtinyxml2.dylib
            )
        endif()

        add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
        include_directories(SYSTEM third_party/tinyxml2)
        link_directories(third_party/tinyxml2)
        set(TINYXML2_LIBRARY tinyxml2)
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

    add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
    include_directories(SYSTEM third_party/tinyxml2)
    link_directories(third_party/tinyxml2)
    set(TINYXML2_LIBRARY tinyxml2)
elseif(MSVC)
    add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
    include_directories(SYSTEM third_party/tinyxml2)
    link_directories(third_party/tinyxml2)
    set(TINYXML2_LIBRARY tinyxml2)
else()
    # For Linux for now we use the system tinyxml2, otherwise the unit test crashes
    # miraculously.
    set(TINYXML2_LIBRARY tinyxml2)
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
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
