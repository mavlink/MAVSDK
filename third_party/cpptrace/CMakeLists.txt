cmake_minimum_required(VERSION 3.1)

project(external-cpptrace)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_STANDARD=17"
    )

message(STATUS "Preparing external project \"cpptrace\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    cpptrace
    DOWNLOAD_DIR ${DOWNLOAD_DIR}
    URL https://github.com/jeremy-rifkin/cpptrace/archive/refs/tags/v0.6.2.tar.gz
    URL_MD5 8a04f30985f6d7a94841c94f29e1fd65
    PREFIX cpptrace
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
