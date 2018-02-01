# Build libdronecore

# This finds thread libs on Linux, Mac, and Windows.
find_package(Threads REQUIRED)

include_directories(
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

if(IOS OR ANDROID OR MSVC OR APPLE)
    set(library_type "STATIC")

    # We need tinyxml2 for the camera definition parsing.
    # We use the submodule when linking statically.
    add_subdirectory(libs/tinyxml2 EXCLUDE_FROM_ALL)
    include_directories(SYSTEM libs/tinyxml2)
else()
    set(library_type "SHARED")
endif()

if(ANDROID)
    set(CURL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/core/external/curl-android-ios/prebuilt-with-ssl/android/include)
    set(CURL_LIBRARY ${CMAKE_SOURCE_DIR}/core/external/curl-android-ios/prebuilt-with-ssl/android/${ANDROID_ABI}/libcurl.a)
elseif(IOS)
    set(CURL_INCLUDE_DIRS external/curl-android-ios/prebuilt-with-ssl/ios/include)
    set(CURL_LIBRARY external/curl-android-ios/prebuilt-with-ssl/ios/libcurl.a)
elseif(MSVC)
    # You need to call cmake with -DWIN_CURL_INCLUDE_DIR:STRING="C:\\curl-7.54.1\\include"
    if(NOT WIN_CURL_INCLUDE_DIR)
        message(FATAL_ERROR "Please provide argument -DWIN_CURL_INCLUDE_DIR:STRING=\"path_to_curl_include\"")
    endif()
    if(NOT WIN_CURL_LIB)
        message(FATAL_ERROR "Please provide argument -DWIN_CURL_LIBSTRING=\"path_to_curl_lib\"")
    endif()

    set(CURL_INCLUDE_DIRS ${WIN_CURL_INCLUDE_DIR})
    set(CURL_LIBRARY ${WIN_CURL_LIB})
else()
    find_package(CURL REQUIRED)
    set(CURL_LIBRARY "curl")
endif()

add_definitions(
    -DCURL_STATICLIB
)

include_directories(${CURL_INCLUDE_DIRS})

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

# cmake should check for C++11
set_property(TARGET dronecore PROPERTY CXX_STANDARD 11)
set_property(TARGET dronecore PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(dronecore
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARY}
    tinyxml2
)

# Link to Windows networking lib.
if (MSVC)
    target_link_libraries(dronecore
        ws2_32
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
    include_directories(SYSTEM libs/gtest/googletest/include)
    include_directories(SYSTEM libs/gtest/googlemock/include)

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

