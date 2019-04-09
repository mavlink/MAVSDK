cmake_minimum_required(VERSION 3.1)

project(external-cares)
include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/c-ares/c-ares.git)
set(ARG_GIT_TAG cares-1_15_0)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCARES_STATIC=ON"
    "-DCARES_SHARED=OFF"
    "-DCARES_STATIC_PIC=ON"
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

message(STATUS "Preparing external project \"c-ares\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

ExternalProject_add(
    cares
    GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
    GIT_TAG ${ARG_GIT_TAG}
    GIT_SHALLOW TRUE
    PREFIX cares
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "${CMAKE_ARGS}" <SOURCE_DIR>
    )
