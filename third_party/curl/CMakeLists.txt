cmake_minimum_required(VERSION 3.1)

project(external-curl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCURL_USE_OPENSSL=OFF"
    "-DCURL_USE_LIBSSH2=OFF"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCURL_CA_PATH_SET=OFF"
    "-DCURL_ZLIB=OFF"
    "-DHTTP_ONLY=ON"
    "-DHAVE_POSIX_STRERROR_R=1"
    "-DBUILD_TESTING=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DBUILD_CURL_EXE=OFF"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DBUILD_CURL_EXE=OFF"
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

message(STATUS "Preparing external project \"curl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-7_88_1
    GIT_SHALLOW ON
    PREFIX curl
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
