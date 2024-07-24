cmake_minimum_required(VERSION 3.1)

project(external-absl)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_CXX_STANDARD=17"
    "-DBUILD_TESTING=OFF"
    "-DABSL_BUILD_TESTING=OFF"
    "-DABSL_PROPAGATE_CXX_STD=ON"
    )

if(IOS)
    list(APPEND CMAKE_ARGS
        "-DPLATFORM=${PLATFORM}"
        "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}"
        "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}"
        )
endif()

message(STATUS "Preparing external project \"absl\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG cd75cb4ae32c46c84cef9a9c78b42174f22ed0ac
    PREFIX absl
    PATCH_COMMAND git checkout . && git apply ${PROJECT_SOURCE_DIR}/rpi-no-crypto.patch
    CMAKE_ARGS "${CMAKE_ARGS}"
    )
