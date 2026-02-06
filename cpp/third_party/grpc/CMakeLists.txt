cmake_minimum_required(VERSION 3.13)

project(external-grpc)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_STANDARD=17"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DRUN_HAVE_STD_REGEX=0"
    "-DRUN_HAVE_POSIX_REGEX=0"
    "-DRUN_HAVE_STEADY_CLOCK=0"
    "-DgRPC_BUILD_TESTS=OFF"
    "-DgRPC_ZLIB_PROVIDER=package"
    "-DgRPC_CARES_PROVIDER=package"
    "-DgRPC_PROTOBUF_PROVIDER=package"
    "-DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG"
    "-DgRPC_RE2_PROVIDER=package"
    "-DgRPC_SSL_PROVIDER=package"
    "-DgRPC_ABSL_PROVIDER=package"
    "-DgRPC_INSTALL_default=ON"
    "-DgRPC_MSVC_STATIC_RUNTIME=OFF"
    "-DgRPC_BUILD_CSHARP_EXT=OFF"
    "-DgRPC_DOWNLOAD_ARCHIVES=OFF"
    )

if(CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_ARGS
        "-DgRPC_BUILD_CODEGEN=OFF"
        )
endif()

if(MANYLINUX1)
    list(APPEND CMAKE_ARGS
        "-DCMAKE_CXX_FLAGS=-DGPR_MANYLINUX1=1"
        )
endif()

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"grpc\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

if(NOT MSVC)
    set(MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules/non-msvc")
    message(STATUS "cmake modules path: ${MODULES_PATH}")

    list(APPEND CMAKE_ARGS
        "-DCMAKE_MODULE_PATH=${MODULES_PATH}"
    )
endif()

ExternalProject_add(
    grpc
    URL https://github.com/grpc/grpc/archive/v1.68.2.tar.gz
    PREFIX grpc
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
