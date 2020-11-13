cmake_minimum_required(VERSION 3.1)

project(external-curl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DBUILD_CURL_EXE=OFF"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DBUILD_CURL_EXE=OFF"
        )
endif()

message(STATUS "Preparing external project \"re2\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    re2
    URL https://github.com/google/re2/archive/2020-11-01.tar.gz
    PREFIX re2
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
