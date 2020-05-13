cmake_minimum_required(VERSION 3.1)

project(external-absl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    )

message(STATUS "Preparing external project \"absl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20200225
    PREFIX absl
    PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/no-xray-attributes.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
