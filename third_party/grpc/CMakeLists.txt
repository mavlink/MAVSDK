cmake_minimum_required(VERSION 3.1)

project(external-grpc)
include(ExternalProject)

# This answer probably saved me from destroying my computer:
# https://stackoverflow.com/questions/45414507/pass-a-list-of-prefix-paths-to-externalproject-add-in-cmake-args
string(REPLACE ";" "|" CMAKE_PREFIX_PATH_ALT_SEP "${CMAKE_PREFIX_PATH}")

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_ALT_SEP}"
    "-DZLIB_ROOT=${CMAKE_INSTALL_PREFIX}"
    "-DOPENSSL_ROOT_DIR=${CMAKE_INSTALL_PREFIX}"
    "-DgRPC_BUILD_TESTS=OFF"
    "-DgRPC_ZLIB_PROVIDER=package"
    "-DgRPC_CARES_PROVIDER=package"
    "-DgRPC_PROTOBUF_PROVIDER=package"
    "-DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG"
    "-DgRPC_SSL_PROVIDER=package"
    "-DgRPC_INSTALL_default=ON"
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

message(STATUS "Preparing external project \"grpc\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    grpc
    URL https://github.com/dronecore/grpc/archive/fix-pointer-not-aligned-ios.zip
    PREFIX grpc
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "${CMAKE_ARGS}" <SOURCE_DIR>
    )
