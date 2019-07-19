cmake_minimum_required(VERSION 3.1)

project(external-libressl)
include(ExternalProject)

set(LIBRESSL_VERSION 2.9.1)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DLIBRESSL_APPS=OFF"
    "-DLIBRESSL_TESTS=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DANDROID_ABI:STRING=${ANDROID_ABI}"
        "-DANDROID_STL:STRING=${ANDROID_STL}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        )
elseif(IOS)
    set(LIBRESSL_VERSION 2.9.0)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DCMAKE_C_FLAGS=-Wno-implicit-function-declaration"
        )
endif()

message(STATUS "Preparing external project \"libressl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    libressl
    URL https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-${LIBRESSL_VERSION}.tar.gz
    PREFIX libressl
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
