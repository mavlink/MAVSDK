cmake_minimum_required(VERSION 3.13)

project(external-mavlink)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    )

# Without this GitHub actions picks the wrong python3 executable.
set(Python3_FIND_REGISTRY "NEVER")
find_package(Python3 COMPONENTS Interpreter REQUIRED)

ExternalProject_add(
    mavlink
    GIT_REPOSITORY https://github.com/mavlink/mavlink
    GIT_TAG 3b52eac09c2e37325e4bc49cd2667ea37bf1d7d2
    GIT_SHALLOW TRUE
    PREFIX mavlink
    CONFIGURE_COMMAND Python3::Interpreter
        -m pymavlink.tools.mavgen
        --lang=C
        --wire-protocol=2.0
        --output=<BINARY_DIR>/include/mavlink/v2.0/
        message_definitions/v1.0/${MAVLINK_DIALECT}.xml
    BUILD_COMMAND "" 
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE TRUE
)

#ExternalProject_Add_Step(mavlink generation
#    WORKING_DIRECTORY <SOURCE_DIR>
#    COMMENT "Generate mavlink headers"
#    ALWAYS TRUE
#)
