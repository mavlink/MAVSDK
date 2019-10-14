cmake_minimum_required(VERSION 3.1)

project(external-grpc)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DZLIB_ROOT=${CMAKE_INSTALL_PREFIX}"
    "-DOPENSSL_ROOT_DIR=${CMAKE_INSTALL_PREFIX}"
    "-DRUN_HAVE_STD_REGEX=0"
    "-DRUN_HAVE_POSIX_REGEX=0"
    "-DRUN_HAVE_STEADY_CLOCK=0"
    "-DgRPC_BUILD_TESTS=OFF"
    "-DgRPC_ZLIB_PROVIDER=package"
    "-DgRPC_CARES_PROVIDER=package"
    "-DgRPC_PROTOBUF_PROVIDER=package"
    "-DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG"
    "-DgRPC_SSL_PROVIDER=package"
    "-DgRPC_INSTALL_default=ON"
    "-DgRPC_MSVC_STATIC_RUNTIME=OFF"
    "-DgRPC_BUILD_CSHARP_EXT=OFF"
    )

if(CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_ARGS
        "-DgRPC_BUILD_CODEGEN=OFF"
        )
endif()

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DANDROID_ABI:STRING=${ANDROID_ABI}"
        "-DANDROID_STL:STRING=${ANDROID_STL}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DgRPC_BUILD_CODEGEN=OFF"
        )
elseif(MANYLINUX1)
    list(APPEND CMAKE_ARGS
        "-DCMAKE_CXX_FLAGS=-DGPR_MANYLINUX1=1"
        )
endif()

message(STATUS "Preparing external project \"grpc\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc
    GIT_TAG v1.24.0
    PREFIX grpc
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
