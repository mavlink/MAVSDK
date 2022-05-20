cmake_minimum_required(VERSION 3.1)

project(external-re2)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DBUILD_SHARED_LIBS=OFF"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"re2\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    re2
    URL https://github.com/google/re2/archive/2022-04-01.tar.gz
    PREFIX re2
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
