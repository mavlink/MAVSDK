cmake_minimum_required(VERSION 3.1)

project(external-jsoncpp)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DJSONCPP_WITH_CMAKE_PACKAGE=ON"
    "-DJSONCPP_WITH_TESTS=OFF"
    )

message(STATUS "Preparing external project \"jsoncpp\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_Add(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
    GIT_TAG 1.8.4
    PREFIX jsoncpp
    PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/fixlibname.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
