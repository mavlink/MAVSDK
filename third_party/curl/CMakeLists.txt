cmake_minimum_required(VERSION 3.1)

project(external-curl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/install"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_USE_OPENSSL=OFF"
    "-DCMAKE_USE_LIBSSH2=OFF"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCURL_CA_PATH_SET=OFF"
    "-DCURL_ZLIB=OFF"
    "-DHTTP_ONLY=ON"
    "-DHAVE_POSIX_STRERROR_R=1"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DANDROID_ABI:STRING=${ANDROID_ABI}"
        "-DANDROID_STL:STRING=${ANDROID_STL}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DIOS_PLATFORM=${IOS_PLATFORM}"
        )
endif()

message(STATUS "Preparing external project \"curl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    curl
    URL https://github.com/curl/curl/releases/download/curl-7_64_1/curl-7.64.1.tar.xz
    PREFIX curl
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "${CMAKE_ARGS}" <SOURCE_DIR>
    )
