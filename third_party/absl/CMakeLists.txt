cmake_minimum_required(VERSION 3.1)

project(external-absl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DABSL_NO_XRAY_ATTRIBUTES=ON"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    )

message(STATUS "Preparing external project \"absl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    absl
    URL https://github.com/abseil/abseil-cpp/archive/20200225.tar.gz
    PREFIX absl
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
