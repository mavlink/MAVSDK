cmake_minimum_required(VERSION 2.8.12)

project(dronelink)

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# We want C++11 and lots of warnings.
add_definitions("-std=c++11 -Wall -Wextra -Weffc++ -Werror")

# Clang needs this warning disabled.
if (APPLE OR IOS)
    add_definitions("-Wno-missing-braces")
endif()

# Autogenerate the device_plugin_container.{h|cpp} to include all plugins.
include(autogenerate_plugin_container.cmake)

# Header files in include denote public facing header files, the rest is in
# src or the respective plugin directories.
include_directories(
    include
    libs/include
    src
    ${plugins}
    ${external_plugins}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

# We build one static library.
add_library(dronelink STATIC
    src/global_include.cpp
    src/connection.cpp
    src/device.cpp
    src/device_impl.cpp
    src/dronelink.cpp
    src/dronelink_impl.cpp
    src/mavlink_channels.cpp
    src/mavlink_receiver.cpp
    src/serial_connection.cpp
    src/tcp_connection.cpp
    src/udp_connection.cpp
    src/plugin_impl_base.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/device_plugin_container.cpp
    ${plugin_source_files}
)

# We support install in order to use the header and static library files in
# other applications.
install(TARGETS dronelink
    DESTINATION "lib"
)
install(FILES
    include/dronelink.h
    include/error_handling.h
    include/device.h
    src/plugin_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/device_plugin_container.h
    ${plugin_header_paths}
    DESTINATION "include/dronelink"
)

if(NOT IOS AND NOT APPLE)
    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR)
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(unit_tests
        src/global_include_test.cpp
        src/mavlink_channels_test.cpp
        src/unittests_main.cpp
    )

    target_link_libraries(unit_tests
        dronelink
        ${GTEST_BOTH_LIBRARIES}
        pthread
    )

    add_test(tests
        unit_tests
    )

    # `make test` does not show output, but `make check` does
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
endif()
