cmake_minimum_required(VERSION 3.22.1)

project(external-openssl)
include(ExternalProject)

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
  set(BUILD_PARALLEL_JOBS ${N})
else()
  set(BUILD_PARALLEL_JOBS 4) # fallback if detection fails
endif()

message(STATUS "Building with ${BUILD_PARALLEL_JOBS} jobs")

message(STATUS "Preparing external project \"openssl\"")

set(OPENSSL_VERSION 3.6.0)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(OPENSSL_BUILD_TYPE "--debug")
else()
    set(OPENSSL_BUILD_TYPE "--release")
endif()
message("OPENSSL_BUILD_TYPE set to ${OPENSSL_BUILD_TYPE}")

if(ANDROID)
    if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
        set(OPENSSL_PLATFORM "android-arm")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
        set(OPENSSL_PLATFORM "android-arm64")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86")
        set(OPENSSL_PLATFORM "android-x86")
    elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86_64")
        set(OPENSSL_PLATFORM "android-x86_64")
    else()
        message(FATAL_ERROR "Unexpected PLATFORM: \"${PLATFORM}\"")
    endif()

    message("OPENSSL_PLATFORM set to ${OPENSSL_PLATFORM}")


    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
        PREFIX openssl
        PATCH_COMMAND patch -p 0 < ${PROJECT_SOURCE_DIR}/dockcross-android.patch
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env ANDROID_NDK=$ENV{CROSS_ROOT} <SOURCE_DIR>/Configure --sysroot=$ENV{CROSS_ROOT}/sysroot ${OPENSSL_PLATFORM} --prefix=${CMAKE_INSTALL_PREFIX} ${OPENSSL_BUILD_TYPE} no-asm no-shared
        BUILD_COMMAND make -j${BUILD_PARALLEL_JOBS} build_libs
        INSTALL_COMMAND make install_dev
        )

elseif(IOS)
    if(${PLATFORM} STREQUAL "OS64")
        set(OPENSSL_PLATFORM "ios64-xcrun")
    elseif(${PLATFORM} STREQUAL "SIMULATOR64")
        set(OPENSSL_PLATFORM "iossimulator-xcrun")
    elseif(${PLATFORM} STREQUAL "SIMULATORARM64")
        set(OPENSSL_PLATFORM "iossimulator-arm64-xcrun")
    else()
        message(FATAL_ERROR "Unexpected PLATFORM: \"${PLATFORM}\"")
    endif()

    message("OPENSSL_PLATFORM set to ${OPENSSL_PLATFORM}")

    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
        PREFIX openssl
        CONFIGURE_COMMAND <SOURCE_DIR>/Configure ${OPENSSL_PLATFORM} --prefix=${CMAKE_INSTALL_PREFIX} ${OPENSSL_BUILD_TYPE} no-shared no-tests
        )

elseif(CMAKE_CROSSCOMPILING AND NOT MSVC) # This assumes we use dockcross
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
        set(OPENSSL_PLATFORM "linux-armv4")
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        set(OPENSSL_PLATFORM "linux-aarch64")
    else()
        message(FATAL_ERROR "Unexpected PLATFORM: \"${PLATFORM}\"")
    endif()

    message("OPENSSL_PLATFORM set to ${OPENSSL_PLATFORM}")

    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
        PREFIX openssl
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env CROSS_COMPILE="" <SOURCE_DIR>/Configure ${OPENSSL_PLATFORM} --prefix=${CMAKE_INSTALL_PREFIX} ${OPENSSL_BUILD_TYPE} no-shared no-tests
        BUILD_COMMAND make -j${BUILD_PARALLEL_JOBS} build_libs
        INSTALL_COMMAND make install_dev
        )

elseif(MSVC)
    message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
    cmake_path(NATIVE_PATH CMAKE_INSTALL_PREFIX NORMALIZE NATIVE_INSTALL_PREFIX)
    message("NATIVE_INSTALL_PREFIX: ${NATIVE_INSTALL_PREFIX}")

    # Determine OpenSSL platform based on architecture
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(OPENSSL_PLATFORM "VC-WIN64A")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(OPENSSL_PLATFORM "VC-WIN32")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(OPENSSL_PLATFORM "VC-WIN64-ARM")
    else()
        message(FATAL_ERROR "Unsupported Windows architecture: ${CMAKE_GENERATOR_PLATFORM}")
    endif()
    message("OPENSSL_PLATFORM set to ${OPENSSL_PLATFORM}")

    find_program(JOM_EXECUTABLE
        NAMES jom jom.exe
        DOC "Path to jom executable"
    )

    if(JOM_EXECUTABLE)
        message(STATUS "Using jom for OpenSSL parallel build")
        # Parallel build of openssl is not supported unless you use jom.
        # See: https://github.com/openssl/openssl/issues/9931
        ExternalProject_add(
            openssl
            URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
            PREFIX openssl
            CONFIGURE_COMMAND perl <SOURCE_DIR>/Configure ${OPENSSL_PLATFORM} --prefix=${NATIVE_INSTALL_PREFIX} --openssldir=${NATIVE_INSTALL_PREFIX} ${OPENSSL_BUILD_TYPE} no-shared no-asm no-tests /FS
            BUILD_COMMAND set MAKEFLAGS= && set set CL=/O2 /MT /DNDEBUG /FS && jom -j ${BUILD_PARALLEL_JOBS} build_libs
            INSTALL_COMMAND set MAKEFLAGS= && nmake install_dev
            )
    else()
        message(STATUS "Falling back to nmake for OpenSSL build using single core")
           ExternalProject_add(
            openssl
            URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
            PREFIX openssl
            CONFIGURE_COMMAND perl <SOURCE_DIR>/Configure ${OPENSSL_PLATFORM} --prefix=${NATIVE_INSTALL_PREFIX} --openssldir=${NATIVE_INSTALL_PREFIX} ${OPENSSL_BUILD_TYPE} no-shared no-asm no-tests
            BUILD_COMMAND set MAKEFLAGS= && nmake build_libs
            INSTALL_COMMAND set MAKEFLAGS= && nmake install_dev
        )
    endif()

else()
    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz
        PREFIX openssl
        CONFIGURE_COMMAND <SOURCE_DIR>/config ${OPENSSL_BUILD_TYPE} --prefix=${CMAKE_INSTALL_PREFIX} --libdir=lib no-shared
        BUILD_COMMAND make -j${BUILD_PARALLEL_JOBS} build_libs
        INSTALL_COMMAND make install_dev
        )
endif()
