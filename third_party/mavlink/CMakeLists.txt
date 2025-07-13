cmake_minimum_required(VERSION 3.13)

project(external-mavlink)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
    "-DMAVLINK_DIALECT=${MAVLINK_DIALECT}"
    )

set(MAVLINK_GIT_HASH 5e3a42b8f3f53038f2779f9f69bd64767b913bb8)

if(IOS)
    ExternalProject_add(
        mavlink
        GIT_REPOSITORY https://github.com/mavlink/mavlink
        GIT_TAG ${MAVLINK_GIT_HASH}
        PREFIX mavlink
        PATCH_COMMAND git checkout .
        COMMAND git apply ${PROJECT_SOURCE_DIR}/mavlink.patch
        COMMAND git apply ${PROJECT_SOURCE_DIR}/mavlink_ios.patch
        CMAKE_ARGS "${CMAKE_ARGS}"
        INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/message_definitions/v1.0/minimal.xml ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/message_definitions/v1.0/standard.xml ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/message_definitions/v1.0/common.xml ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0/
    )
else()
    ExternalProject_add(
        mavlink
        GIT_REPOSITORY https://github.com/mavlink/mavlink
        GIT_TAG ${MAVLINK_GIT_HASH}
        PREFIX mavlink
        PATCH_COMMAND git checkout . && git apply ${PROJECT_SOURCE_DIR}/mavlink.patch
        CMAKE_ARGS "${CMAKE_ARGS}"
        INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/message_definitions/v1.0/minimal.xml ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/message_definitions/v1.0/standard.xml ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0/
                COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/message_definitions/v1.0/common.xml ${CMAKE_INSTALL_PREFIX}/include/mavlink/message_definitions/v1.0/
    )
endif()
