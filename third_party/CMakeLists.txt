cmake_minimum_required(VERSION 3.1)

project(third_party)

include(cmake/build_target.cmake)

list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

build_target(json11)
if(NOT TINYXML2_FOUND)
build_target(tinyxml2)
endif()

if(NOT IOS AND NOT ZLIB_FOUND)
build_target(zlib)
endif()
if(NOT CURL_FOUND)
build_target(curl)
endif()

if(${BUILD_BACKEND})
    build_target(libressl)
    build_target(cares)
    build_target(protobuf)
    build_target(grpc)
endif()
