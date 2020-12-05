cmake_minimum_required(VERSION 3.1)

project(external-openssl)
include(ExternalProject)

message(STATUS "Preparing external project \"openssl\"")

if(ANDROID)
    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-1.1.1h.tar.gz
        PREFIX openssl
        PATCH_COMMAND patch -p 0 < ${PROJECT_SOURCE_DIR}/dockcross-android.patch
        CONFIGURE_COMMAND <SOURCE_DIR>/Configure --sysroot=$ENV{CROSS_ROOT}/sysroot $ENV{OPENSSL_PLATFORM} --prefix=${CMAKE_INSTALL_PREFIX} no-shared
        BUILD_COMMAND make
        )
elseif(IOS)
    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-1.1.1h.tar.gz
        PREFIX openssl
        CONFIGURE_COMMAND <SOURCE_DIR>/Configure ios64-cross --prefix=${CMAKE_INSTALL_PREFIX} no-shared
        )
elseif(CMAKE_CROSSCOMPILING) # This assumes we use dockcross
    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-1.1.1h.tar.gz
        PREFIX openssl
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env CROSS_COMPILE="" <SOURCE_DIR>/Configure $ENV{OPENSSL_PLATFORM} --prefix=${CMAKE_INSTALL_PREFIX} no-shared
        BUILD_COMMAND make
        )
else()
    ExternalProject_add(
        openssl
        URL https://www.openssl.org/source/openssl-1.1.1h.tar.gz
        PREFIX openssl
        CONFIGURE_COMMAND <SOURCE_DIR>/config $ENV{OPENSSL_PLATFORM} --prefix=${CMAKE_INSTALL_PREFIX} no-shared
        )
endif()
