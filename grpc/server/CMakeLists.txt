cmake_minimum_required(VERSION 2.8.12)

#project(grpcdronecore)

include(FindPkgConfig REQUIRED)
pkg_check_modules(GRPC grpc++)

include(FindProtobuf REQUIRED)

add_definitions(
    -std=c++11
    -Wno-unused-parameter
)

set(proto_dir ${CMAKE_SOURCE_DIR}/grpc/proto)

add_custom_command(OUTPUT dronecore.grpc.pb.cc
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} -I ${proto_dir}
        --grpc_out=.
        --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`
        ${proto_dir}/dronecore.proto;
)

add_custom_command(OUTPUT dronecore.pb.cc
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} -I ${proto_dir}
        --cpp_out=. ${proto_dir}/dronecore.proto
)

add_executable(dronecore_server
    dronecore_server.cpp
    dronecore.pb.cc # generated
    dronecore.grpc.pb.cc # generated
)

target_link_libraries(dronecore_server
    dronecore
    pthread
    protobuf
    ${GRPC_LIBRARIES}
    grpc++_reflection
    dl
)

include_directories(
    ${CMAKE_BINARY_DIR}/grpc/server
)
