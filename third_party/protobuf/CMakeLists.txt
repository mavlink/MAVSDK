cmake_minimum_required(VERSION 3.1)

project(external-protobuf)
include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/google/protobuf.git)
set(ARG_GIT_TAG v3.6.1)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-Dprotobuf_BUILD_TESTS=OFF"
    "-Dprotobuf_MODULE_COMPATIBLE=ON"
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

message(STATUS "Preparing external project \"protobuf\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    protobuf
    GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
    GIT_TAG ${ARG_GIT_TAG}
    GIT_SHALLOW TRUE
    PREFIX protobuf
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "${CMAKE_ARGS}" <SOURCE_DIR>/cmake
    )
