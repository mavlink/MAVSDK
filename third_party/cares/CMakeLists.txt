cmake_minimum_required(VERSION 3.1)

project(external-cares)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCARES_STATIC=ON"
    "-DCARES_SHARED=OFF"
    "-DCARES_STATIC_PIC=ON"
    "-DCARES_BUILD_TOOLS=OFF"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"c-ares\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    cares
    URL https://github.com/c-ares/c-ares/releases/download/cares-1_19_1/c-ares-1.19.1.tar.gz
    PREFIX cares
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
