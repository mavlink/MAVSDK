cmake_minimum_required(VERSION 3.1)

project(external-ghc-filesystem)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DBUILD_TESTING=OFF"
    "-DBUILD_EXAMPLES=OFF"
    )

if(ANDROID)
    list(APPEND CMAKE_ARGS
        "-DANDROID_ABI:STRING=${ANDROID_ABI}"
        "-DANDROID_STL:STRING=${ANDROID_STL}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        )
elseif(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        )
endif()

message(STATUS "Preparing external project \"ghc-filesystem\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    ghc-filesystem
    GIT_REPOSITORY https://github.com/gulrak/filesystem
    GIT_TAG v1.2.4
    PREFIX ghc-filesystem
    PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/disable_tests.patch && git apply ${PROJECT_SOURCE_DIR}/export_target.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
