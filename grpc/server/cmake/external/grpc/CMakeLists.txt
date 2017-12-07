cmake_minimum_required(VERSION 2.8)

project(external-grpc)

include(ExternalProject)

ExternalProject_add(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG v1.7.x
    PREFIX grpc
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=$ENV{DRONECORESERVER_GRPC_BINARY_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DgRPC_BUILD_TESTS=OFF
        -DgRPC_ZLIB_PROVIDER=package
        -DgRPC_CARES_PROVIDER=package
        -DgRPC_PROTOBUF_PROVIDER=package
        -DgRPC_PROTOBUF_PACKAGE_TYPE=CONFIG
        -DgRPC_SSL_PROVIDER=package
        -DgRPC_INSTALL_default=ON
    UPDATE_COMMAND ""
)
