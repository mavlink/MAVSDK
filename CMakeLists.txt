cmake_minimum_required(VERSION 2.8.12)

project(dronelink)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Weffc++ -Werror")

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

add_library(dronelink STATIC
    src/connection.cpp
    src/device.cpp
    src/device_impl.cpp
    src/info.cpp
    src/info_impl.cpp
    src/telemetry.cpp
    src/telemetry_impl.cpp
    src/dronelink.cpp
    src/dronelink_impl.cpp
    src/error_handling.cpp
    src/mavlink_channels.cpp
    src/mavlink_receiver.cpp
    src/serial_connection.cpp
    src/tcp_connection.cpp
    src/udp_connection.cpp
    src/math_conversions.cpp
)

include_directories(
    include
    libs/include
)

if(NOT IOS)
    add_subdirectory(integration_tests)

    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(unit_tests
        src/mavlink_channels_test.cpp
        src/unittests_main.cpp
    )

    target_link_libraries(unit_tests
        dronelink
        ${GTEST_BOTH_LIBRARIES}
        pthread
        atomic
    )

    add_test(tests
        unit_tests
    )

    # `make test` does not show output, but `make check` does
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
endif()
