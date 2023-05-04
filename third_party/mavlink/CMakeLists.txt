cmake_minimum_required(VERSION 3.13)

project(external-mavlink)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DMAVLINK_DIALECT=${MAVLINK_DIALECT}"
    )

ExternalProject_add(
    mavlink
    GIT_REPOSITORY https://github.com/mavlink/mavlink
    GIT_TAG ce00667f1e1eb3e683697e817464f7d8309e3f72
    PREFIX mavlink
    CMAKE_ARGS "${CMAKE_ARGS}"
)
