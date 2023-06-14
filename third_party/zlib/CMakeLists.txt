cmake_minimum_required(VERSION 3.1)

project(external-zlib)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"zlib\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib
    GIT_TAG v1.2.13
    PREFIX zlib
    PATCH_COMMAND git checkout . && git apply ${PROJECT_SOURCE_DIR}/build_shared_libs.patch && git apply ${PROJECT_SOURCE_DIR}/bump_cmake_min_version.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
