find_package(Threads REQUIRED)

include_directories(
    ${DRONECORE_ZLIB_INCLUDE_DIRS}
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/mavlink/include
)

if(IOS OR MSVC)
    set(library_type "STATIC")
else()
    set(library_type "SHARED")
endif()

if(IOS OR MSVC OR APPLE)
    # We need tinyxml2 for the camera definition parsing.
    add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
    include_directories(SYSTEM third_party/tinyxml2)
elseif(ANDROID)
    # We need tinyxml2 for the camera definition parsing.
    add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
endif()

add_library(dronecore ${library_type}
    call_every_handler.cpp
    connection.cpp
    curl_wrapper.cpp
    device.cpp
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
    tinyxml2
    ${DRONECORE_ZLIB_LIBRARIES}
)

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
    dronecore.h
    device.h
    plugin_base.h
    ${plugin_header_paths}
    DESTINATION "include/dronecore"
)


if(NOT IOS AND NOT ANDROID)
    # SYSTEM because we don't want warnings for gtest headers.
    include_directories(SYSTEM third_party/gtest/googletest/include)
    include_directories(SYSTEM third_party/gtest/googlemock/include)

    add_executable(unit_tests_runner
        global_include_test.cpp
        mavlink_channels_test.cpp
        unittests_main.cpp
        # TODO: add this again
        #core/http_loader_test.cpp
        timeout_handler_test.cpp
        call_every_handler_test.cpp
        curl_test.cpp
        ${plugin_unittest_source_files}
    )

    if (MSVC)
        # We need this to prevent linking errors from happening in the Windows build.
        target_compile_definitions(unit_tests_runner PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
        target_compile_options(unit_tests_runner PUBLIC "/wd4251" "/wd4275")
    endif()

    target_compile_definitions(unit_tests_runner PRIVATE FAKE_TIME=1)

    set_target_properties(unit_tests_runner
        PROPERTIES COMPILE_FLAGS ${warnings}
    )

    target_link_libraries(unit_tests_runner
        dronecore
        gtest
        gtest_main
        gmock
    )

    add_test(unit_tests
        unit_tests_runner
    )
endif()

