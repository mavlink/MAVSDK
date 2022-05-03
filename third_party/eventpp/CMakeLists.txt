cmake_minimum_required(VERSION 3.1)

project(external-eventpp)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DBUILD_SHARED_LIBS=OFF"
    )

message(STATUS "Preparing external project \"eventpp\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    eventpp
    URL https://github.com/wqking/eventpp/archive/refs/tags/v0.1.2.tar.gz
    PREFIX eventpp
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
