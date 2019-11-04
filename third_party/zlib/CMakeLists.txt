cmake_minimum_required(VERSION 3.1)

project(external-zlib)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    )

message(STATUS "Preparing external project \"zlib\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib
    GIT_TAG v1.2.11
    PREFIX zlib
    PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/build_shared_libs.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
