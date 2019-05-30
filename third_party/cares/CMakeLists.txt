cmake_minimum_required(VERSION 3.1)

project(external-cares)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCARES_STATIC=ON"
    "-DCARES_SHARED=OFF"
    "-DCARES_STATIC_PIC=ON"
    "-DCARES_BUILD_TOOLS=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DANDROID_ABI:STRING=${ANDROID_ABI}"
        "-DANDROID_STL:STRING=${ANDROID_STL}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        )
endif()

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
