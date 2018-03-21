cmake_minimum_required(VERSION 3.1)

project(external-protobuf)

include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/google/protobuf.git)
set(ARG_GIT_TAG ab95b1bc1eb964ee5f1fb48297344c5d37d35191)

if(ANDROID)
    message(STATUS "Preparing external project \"protobuf\" for Android...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- ANDROID_ABI: ${ANDROID_ABI}")
    message(STATUS " -- ANDROID_STL: ${ANDROID_STL}")

    ExternalProject_add(
        protobuf
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX protobuf
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DANDROID_ABI:STRING=${ANDROID_ABI}
            -DANDROID_STL:STRING=${ANDROID_STL}
            -DANDROID_PLATFORM=${ANDROID_PLATFORM}
            -Dprotobuf_BUILD_PROTOC_BINARIES=OFF
            -Dprotobuf_BUILD_TESTS=OFF
            -Dprotobuf_BUILD_SHARED_LIBS=OFF
            -Dprotobuf_WITH_ZLIB=OFF
            -Dprotobuf_MODULE_COMPATIBLE=ON
        <SOURCE_DIR>/cmake)
elseif(IOS)
    message(STATUS "Preparing external project \"protobuf\" for iOS...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- IOS_PLATFORM: ${IOS_PLATFORM}")

    ExternalProject_add(
        protobuf
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX protobuf
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DIOS_PLATFORM=${IOS_PLATFORM}
            -DRUN_HAVE_STD_REGEX=0
            -DRUN_HAVE_POSIX_REGEX=0
            -DRUN_HAVE_STEADY_CLOCK=0
            -Dprotobuf_BUILD_PROTOC_BINARIES=OFF
            -Dprotobuf_BUILD_TESTS=OFF
            -Dprotobuf_BUILD_SHARED_LIBS=OFF
            -Dprotobuf_WITH_ZLIB=OFF
            -Dprotobuf_MODULE_COMPATIBLE=ON
        <SOURCE_DIR>/cmake)
else()
    message(STATUS "Preparing external project \"protobuf\"...")

    ExternalProject_add(
        protobuf
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX protobuf
        CONFIGURE_COMMAND ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -Dprotobuf_BUILD_TESTS=OFF
            -Dprotobuf_BUILD_SHARED_LIBS=OFF
            -Dprotobuf_WITH_ZLIB=OFF
            -Dprotobuf_MODULE_COMPATIBLE=ON
        <SOURCE_DIR>/cmake)
endif()
