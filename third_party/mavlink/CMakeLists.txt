cmake_minimum_required(VERSION 3.13)

project(external-mavlink)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    )

# Without this GitHub actions picks the wrong python3 executable.
set(Python3_FIND_REGISTRY "NEVER")
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (NOT MAVLINK_DIALECT)
    set(MAVLINK_DIALECT common)
endif()

message(STATUS "Preparing external project \"mavlink\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    mavlink
    GIT_REPOSITORY https://github.com/mavlink/mavlink
    GIT_TAG 0b5b5dabf9c6ae22f2f93f0db9a4e19dcf6f19ac
    PREFIX mavlink
    CONFIGURE_COMMAND Python3::Interpreter
        -m pymavlink.tools.mavgen
        --lang=C
        --wire-protocol=2.0
        --output=${CMAKE_CURRENT_BINARY_DIR}/intermediate/src/include/mavlink/v2.0/
        message_definitions/v1.0/${MAVLINK_DIALECT}.xml
        && cp ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in ${CMAKE_CURRENT_BINARY_DIR}/intermediate/CMakeLists.txt
        && cp ${CMAKE_CURRENT_SOURCE_DIR}/MAVLinkConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/intermediate/
        && cmake "${CMAKE_ARGS}" -B${CMAKE_CURRENT_BINARY_DIR}/intermediate/build -S${CMAKE_CURRENT_BINARY_DIR}/intermediate
    BUILD_COMMAND cmake --build ${CMAKE_CURRENT_BINARY_DIR}/intermediate/build
    INSTALL_COMMAND cmake --build ${CMAKE_CURRENT_BINARY_DIR}/intermediate/build --target install
    BUILD_IN_SOURCE TRUE
)
