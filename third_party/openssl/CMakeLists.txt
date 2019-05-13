cmake_minimum_required(VERSION 3.1)

project(external-openssl)
include(ExternalProject)

set(ARG_GIT_REPOSITORY https://github.com/openssl/openssl)
set(ARG_GIT_TAG OpenSSL_1_1_1b)

message(STATUS "Preparing external project \"openssl\"")

ExternalProject_add(
    openssl
    GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
    GIT_TAG ${ARG_GIT_TAG}
    PREFIX openssl
    CONFIGURE_COMMAND <SOURCE_DIR>/config --prefix=${CMAKE_INSTALL_PREFIX} no-shared
    )
