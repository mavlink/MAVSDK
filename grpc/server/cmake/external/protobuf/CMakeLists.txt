cmake_minimum_required(VERSION 2.8)

project(external-protobuf)

include(ExternalProject)

ExternalProject_add(
    protobuf
    GIT_REPOSITORY https://github.com/google/protobuf.git
    GIT_TAG v3.5.0
    PREFIX protobuf
    CONFIGURE_COMMAND ${CMAKE_COMMAND}
    -DCMAKE_INSTALL_PREFIX:PATH=$ENV{DRONECORESERVER_PROTOBUF_BINARY_DIR}
      -Dprotobuf_BUILD_TESTS=OFF
      -Dprotobuf_BUILD_SHARED_LIBS=OFF
      -Dprotobuf_WITH_ZLIB=OFF
      -Dprotobuf_MODULE_COMPATIBLE=ON
      <SOURCE_DIR>/cmake
    UPDATE_COMMAND ""
)
