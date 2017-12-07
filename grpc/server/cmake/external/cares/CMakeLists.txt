cmake_minimum_required(VERSION 2.8)

project(external-cares)

include(ExternalProject)

ExternalProject_add(
    cares
    GIT_REPOSITORY https://github.com/c-ares/c-ares.git
    GIT_TAG cares-1_13_0
    PREFIX cares
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=$ENV{DRONECORESERVER_CARES_BINARY_DIR}
        -DCARES_STATIC=ON
        -DCARES_SHARED=OFF
        -DCARES_STATIC_PIC=ON
    UPDATE_COMMAND ""
)
