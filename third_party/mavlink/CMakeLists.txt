cmake_minimum_required(VERSION 3.13)

project(external-mavlink)
include(ExternalProject)

# Without this GitHub actions picks the wrong python3 executable.
set(Python3_FIND_REGISTRY "NEVER")
set(Python3_FIND_FRAMEWORK "NEVER")
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (NOT MAVLINK_DIALECT)
    set(MAVLINK_DIALECT common)
endif()


if(NOT CMAKE_CROSSCOMPILING)
set(CONFIGURE_COMMAND
    Python3::Interpreter -m pip install -r pymavlink/requirements.txt)
endif()

ExternalProject_add(
    mavlink
    GIT_REPOSITORY https://github.com/mavlink/mavlink
    GIT_TAG 3ee5382d0c96134b0e1c250d8c2d54bfed0166fa
    PREFIX mavlink
    CONFIGURE_COMMAND ${CONFIGURE_COMMAND}
    COMMAND Python3::Interpreter
        -m pymavlink.tools.mavgen
        --lang=C
        --wire-protocol=2.0
        --output=${CMAKE_INSTALL_PREFIX}/include/mavlink/v2.0/
        message_definitions/v1.0/${MAVLINK_DIALECT}.xml
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_IN_SOURCE TRUE
)
