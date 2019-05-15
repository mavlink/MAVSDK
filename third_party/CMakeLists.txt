cmake_minimum_required(VERSION 3.1)

project(third_party)

include(cmake/build_target.cmake)

list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

build_target(json11)
build_target(tinyxml2)
build_target(zlib)
build_target(curl)

if(${BUILD_BACKEND})
    build_target(libressl)
    build_target(cares)
    build_target(protobuf)
    build_target(grpc)
endif()
