cmake_minimum_required(VERSION 3.1)

project(external-protobuf)
include(ExternalProject)

list(APPEND CMAKE_ARGS
    "${CL_ARGS}"
    "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
    "-Dprotobuf_BUILD_TESTS=OFF"
    "-Dprotobuf_BUILD_SHARED_LIBS=OFF"
    "-Dprotobuf_MODULE_COMPATIBLE=ON"
    "-Dprotobuf_MSVC_STATIC_RUNTIME=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    )

if(CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_ARGS
        "-Dprotobuf_BUILD_PROTOC_BINARIES=OFF"
        )
endif()

message(STATUS "Preparing external project \"protobuf\" with args:")
foreach(CMAKE_ARG ${CMAKE_ARGS})
    message(STATUS "-- ${CMAKE_ARG}")
endforeach()

# This wouldn't be necessary with CMAKE_ARGS + SOURCE_SUBDIR (see below)
if(DEFINED CMAKE_GENERATOR)
    set(CMAKE_GENERATOR_PARAM "-G${CMAKE_GENERATOR}")
endif()

ExternalProject_add(
    protobuf
    URL https://github.com/protocolbuffers/protobuf/releases/download/v3.11.2/protobuf-cpp-3.11.2.tar.gz
    PREFIX protobuf
    CONFIGURE_COMMAND ${CMAKE_COMMAND} "${CMAKE_GENERATOR_PARAM}" "${CMAKE_ARGS}" <SOURCE_DIR>/cmake
    # CMAKE_ARGS + SOURCE_SUBDIR are more elegant than CONFIGURE_COMMAND,
    # but not supported by CMake 3.5.1 (Ubuntu 16.04)
    #CMAKE_ARGS "${CMAKE_ARGS}"
    #SOURCE_SUBDIR cmake
    )
