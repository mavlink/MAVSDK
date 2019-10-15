cmake_minimum_required(VERSION 3.1)

project(external-tinyxml2)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_BUILD_TYPE=Release"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DBUILD_TESTS=OFF"
    )

message(STATUS "Preparing external project \"tinyxml2\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    tinyxml2
    URL https://github.com/leethomason/tinyxml2/archive/7.1.0.tar.gz
    PREFIX tinyxml2
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
