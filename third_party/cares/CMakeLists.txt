cmake_minimum_required(VERSION 3.1)

project(external-cares)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCARES_STATIC=ON"
    "-DCARES_SHARED=OFF"
    "-DCARES_STATIC_PIC=ON"
    "-DCARES_BUILD_TOOLS=OFF"
    )

message(STATUS "Preparing external project \"c-ares\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    cares
    URL https://github.com/c-ares/c-ares/releases/download/cares-1_15_0/c-ares-1.15.0.tar.gz
    PREFIX cares
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
