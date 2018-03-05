cmake_minimum_required(VERSION 3.1)

project(external-cares)

include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/c-ares/c-ares.git)
set(ARG_GIT_TAG cares-1_13_0)

if(ANDROID)
    message(STATUS "Preparing external project \"c-ares\" for Android...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- ANDROID_ABI: ${ANDROID_ABI}")
    message(STATUS " -- ANDROID_STL: ${ANDROID_STL}")

    ExternalProject_add(
        cares
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX cares
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DANDROID_ABI:STRING=${ANDROID_ABI}
            -DANDROID_STL:STRING=${ANDROID_STL}
            -DANDROID_PLATFORM=${ANDROID_PLATFORM}
            -DCARES_STATIC=ON
            -DCARES_SHARED=OFF
            -DCARES_STATIC_PIC=ON
        UPDATE_COMMAND "")
elseif(IOS)
    message(STATUS "Preparing external project \"c-ares\" for iOS...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- IOS_PLATFORM: ${IOS_PLATFORM}")

    ExternalProject_add(
        cares
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX cares
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DIOS_PLATFORM=${IOS_PLATFORM}
            -DCARES_STATIC=ON
            -DCARES_SHARED=OFF
            -DCARES_STATIC_PIC=ON
        UPDATE_COMMAND "")
else()
    message(STATUS "Preparing external project \"c-ares\"...")

    ExternalProject_add(
        cares
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX cares
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCARES_STATIC=ON
            -DCARES_SHARED=OFF
            -DCARES_STATIC_PIC=ON
        UPDATE_COMMAND "")
endif()
