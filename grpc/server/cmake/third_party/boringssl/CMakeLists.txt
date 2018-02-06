cmake_minimum_required(VERSION 3.1)

project(external-boringssl)

include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/google/boringssl.git)
set(ARG_GIT_TAG 38c20fe8d514a5328f5db404e116af8e9136c7f8)

if(ANDROID)
    message(STATUS "Preparing external project \"boringssl\" for Android...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- ANDROID_ABI: ${ANDROID_ABI}")
    message(STATUS " -- ANDROID_STL: ${ANDROID_STL}")

    ExternalProject_add(
        boringssl
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX boringssl
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DANDROID_ABI:STRING=${ANDROID_ABI}
            -DANDROID_STL:STRING=${ANDROID_STL}
        INSTALL_COMMAND true)
elseif(IOS)
    message(STATUS "Preparing external project \"boringssl\" for iOS...")
    message(STATUS " -- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS " -- IOS_PLATFORM: ${IOS_PLATFORM}")

    ExternalProject_add(
        boringssl
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX boringssl
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
            -DIOS_PLATFORM=${IOS_PLATFORM}
            -DOPENSSL_NO_ASM=ON # Note: https://github.com/grpc/grpc/issues/9440
        INSTALL_COMMAND true)
else()
    # We disable ASM optimizations for MSVC and iOS (see above for iOS), which 
    # might have a performance impact on secure communications.
    # This should be benchmarked/evaluated before we decide if it is worth 
    # enabling them or not in our case.
    #
    # More information here:  https://github.com/grpc/grpc/issues/9440
    if (MSVC)
        set(OPENSSL_NO_ASM ON)
    else()
        set(OPENSSL_NO_ASM OFF)
    endif()

    message(STATUS "Preparing external project \"boringssl\"...")

    ExternalProject_add(
        boringssl
        GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
        GIT_TAG ${ARG_GIT_TAG}
        PREFIX boringssl
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${BINARY_DIR}
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DOPENSSL_NO_ASM=${OPENSSL_NO_ASM}
        INSTALL_COMMAND true)
endif()
