cmake_minimum_required(VERSION 2.8.12)

project(dronelink)

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# We want C++11 and lots of warnings.
if(NOT MSVC)
    # Clang and GCC
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
    set(platform_libs "pthread")
else()
    # MSBuild
    add_definitions("-std=c++11 -WX -W2 -DWINDOWS")
    set(platform_libs "Ws2_32.lib")
endif()

# Clang needs this warning disabled.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions("-stdlib=libc++ -Wno-missing-braces")
endif()

if (ANDROID)
    add_definitions("-frtti")
endif()

if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
    add_definitions("-Wno-missing-field-initializers")
endif()

# Set this in a module to add a library.
set (additional_libs "")

# Autogenerate the device_plugin_container.{h|cpp} to include all plugins.
include(autogenerate_plugin_container.cmake)


# Header files in include denote public facing header files, the rest is in
# core or the respective plugin directories.
include_directories(
    include
    libs/include
    core
    ${plugins}
    ${external_plugins}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/core
)

if (ANDROID)
    include_directories(curl-android-ios/prebuilt-with-ssl/android/include)
endif()

# We build one static library.
add_library(dronelink STATIC
    core/global_include.cpp
    core/connection.cpp
    core/device.cpp
    core/device_impl.cpp
    core/mavlink_parameters.cpp
    core/mavlink_commands.cpp
    core/dronelink.cpp
    core/dronelink_impl.cpp
    core/mavlink_channels.cpp
    core/mavlink_receiver.cpp
    core/serial_connection.cpp
    core/tcp_connection.cpp
    core/udp_connection.cpp
    core/plugin_impl_base.cpp
    core/curl_wrapper.cpp
    core/http_loader.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/core/device_plugin_container.cpp
    ${plugin_source_files}
)

# We support install in order to use the header and static library files in
# other applications.
if(ANDROID)
    set(lib_path "lib/android/${ANDROID_ABI}")

    # The old armeabi arch doesn't support promises yet with gcc 4.9.
    if (ANDROID_ABI STREQUAL "armeabi")
        add_definitions(-DNO_PROMISES)
    endif()

elseif(IOS)
    set(lib_path "lib/ios")
else()
    set(lib_path "lib")
endif()

install(TARGETS dronelink
    DESTINATION ${lib_path}
)

install(FILES
    include/dronelink.h
    include/device.h
    core/plugin_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/device_plugin_container.h
    ${plugin_header_paths}
    DESTINATION "include/dronelink"
)

if(NOT IOS AND NOT ANDROID)
    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    enable_testing()

    add_subdirectory(libs/gtest EXCLUDE_FROM_ALL)
    include_directories(libs/gtest/googletest/include)
    include_directories(libs/gtest/googlemock/include)

    if(NOT MSVC)
        # gtest has these warnings
        add_definitions("-Wno-sign-compare")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTESTING")

    add_executable(unit_tests_runner
        core/global_include_test.cpp
        core/mavlink_channels_test.cpp
        core/unittests_main.cpp
        core/http_loader_test.cpp
        ${plugin_unittest_source_files}
        ${unit_tests_src}
    )

    target_link_libraries(unit_tests_runner
        dronelink
        gtest
        gtest_main
        ${platform_libs}
        gmock
        pthread
        curl
        ${additional_libs}
    )

    add_test(unit_tests
        unit_tests_runner
    )

    # `make test` does not show output, but `make check` does
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

    # This includes all GTests that run integration tests
    add_executable(integration_tests_runner
        ${integration_tests_src}
    )

    target_link_libraries(integration_tests_runner
        dronelink
        gtest
        gtest_main
        ${platform_libs}
        pthread
        curl
        ${additional_libs}
    )

    add_test(integration_tests
        integration_tests_runner
    )

    add_custom_command(TARGET integration_tests_runner
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/start_px4_sitl.sh
            ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_command(TARGET integration_tests_runner
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/stop_px4_sitl.sh
            ${CMAKE_CURRENT_BINARY_DIR}
    )

endif()

if (DROP_DEBUG EQUAL 1)
    add_definitions(-DDROP_DEBUG=${DROP_DEBUG})

    add_executable(drop_debug
        debug_helpers/drop_debug_main.cpp
    )

    target_link_libraries(drop_debug
        dronelink
        ${platform_libs}
    )
endif()
