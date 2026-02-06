cmake_minimum_required(VERSION 3.13)

project(external-curl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCURL_USE_OPENSSL=ON"
    "-DCURL_USE_LIBSSH2=OFF"
    "-DCURL_USE_LIBPSL=OFF"
    "-DBUILD_CURL_EXE=OFF"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCURL_BROTLI=OFF"
    "-DCURL_ZLIB=OFF"
    "-DCURL_ZSTD=OFF"
    "-DHTTP_ONLY=OFF"
    "-DUSE_NGHTTP2=OFF"
    "-DBUILD_TESTING=OFF"
    "-DUSE_LIBIDN2=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DCURL_CA_PATH=/system/etc/security/cacerts"
    )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
elseif(CMAKE_CROSSCOMPILING) # This assumes we use dockcross
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
        list(APPEND CMAKE_ARGS
            "-DCMAKE_EXE_LINKER_FLAGS=-Wl,--no-as-needed -latomic -Wl,--as-needed"
            )
    endif()
endif()

if(NOT MSVC)
    set(MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules/non-msvc")
    message(STATUS "cmake modules path: ${MODULES_PATH}")

    list(APPEND CMAKE_ARGS
        "-DCMAKE_MODULE_PATH=${MODULES_PATH}"
    )
endif()

message(STATUS "Preparing external project \"curl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_16_0
    GIT_SHALLOW ON
    PREFIX curl
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
