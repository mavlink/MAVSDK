cmake_minimum_required(VERSION 3.1)

project(external-grpc)

include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/dronecore/grpc.git)
set(ARG_GIT_TAG enable-android-ios-build)

# This answer probably saved me from destroying my computer:
# https://stackoverflow.com/questions/45414507/pass-a-list-of-prefix-paths-to-externalproject-add-in-cmake-args
string(REPLACE ";" "|" CMAKE_PREFIX_PATH_ALT_SEP "${CMAKE_PREFIX_PATH}")
string(REPLACE ";" "|" CMAKE_FIND_ROOT_PATH_ALT_SEP "${CMAKE_FIND_ROOT_PATH}")

if(ANDROID)
    message(STATUS "Preparing external project \"grpc\" for Android...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- ANDROID_ABI: ${ANDROID_ABI}")
    message(STATUS " -- ANDROID_STL: ${ANDROID_STL}")

    ExternalProject_add(
        grpc
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        GIT_SUBMODULES third_party/benchmark third_party/gflags
        PREFIX grpc
        LIST_SEPARATOR |
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${BINARY_DIR}
            -DCMAKE_FIND_ROOT_PATH:STRING=${CMAKE_FIND_ROOT_PATH_ALT_SEP}
            -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH_ALT_SEP}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DANDROID_ABI:STRING=${ANDROID_ABI}
            -DANDROID_STL:STRING=${ANDROID_STL}
            -DRUN_HAVE_STD_REGEX=0
            -DRUN_HAVE_POSIX_REGEX=0
            -DRUN_HAVE_STEADY_CLOCK=0
            -DgRPC_BUILD_CODEGEN=OFF
            -DgRPC_BUILD_TESTS=OFF
            -DgRPC_ZLIB_PROVIDER=package
            -DgRPC_CARES_PROVIDER=package
            -DgRPC_PROTOBUF_PROVIDER=package
            -DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG
            -DgRPC_SSL_PROVIDER=package
            -DOPENSSL_USE_STATIC_LIBS=TRUE
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            -DgRPC_INSTALL_default=ON)
elseif(IOS)
    message(STATUS "Preparing external project \"grpc\" for iOS...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- IOS_PLATFORM: ${IOS_PLATFORM}")

    ExternalProject_add(
        grpc
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        GIT_SUBMODULES third_party/benchmark third_party/gflags
        PREFIX grpc
        LIST_SEPARATOR |
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${BINARY_DIR}
            -DCMAKE_FIND_ROOT_PATH:STRING=${CMAKE_FIND_ROOT_PATH_ALT_SEP}
            -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH_ALT_SEP}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DIOS_PLATFORM=${IOS_PLATFORM}
            -DRUN_HAVE_STD_REGEX=0
            -DRUN_HAVE_POSIX_REGEX=0
            -DRUN_HAVE_STEADY_CLOCK=0
            -DgRPC_BUILD_CODEGEN=OFF
            -DgRPC_BUILD_TESTS=OFF
            -DgRPC_ZLIB_PROVIDER=package
            -DgRPC_CARES_PROVIDER=package
            -DgRPC_PROTOBUF_PROVIDER=package
            -DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG
            -DgRPC_SSL_PROVIDER=package
            -DOPENSSL_USE_STATIC_LIBS=TRUE
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            -DgRPC_INSTALL_default=ON)
else()
    message(STATUS "Preparing external project \"grpc\"...")

    ExternalProject_add(
        grpc
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        GIT_SUBMODULES third_party/benchmark third_party/gflags
        PREFIX grpc
        LIST_SEPARATOR |
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${BINARY_DIR}
            -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH_ALT_SEP}
            -DCMAKE_FIND_ROOT_PATH:STRING=${CMAKE_FIND_ROOT_PATH_ALT_SEP}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DgRPC_BUILD_TESTS=OFF
            -DgRPC_ZLIB_PROVIDER=package
            -DgRPC_CARES_PROVIDER=package
            -DgRPC_PROTOBUF_PROVIDER=package
            -DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG
            -DgRPC_SSL_PROVIDER=package
            -DOPENSSL_USE_STATIC_LIBS=TRUE
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            -DgRPC_INSTALL_default=ON)
endif()
