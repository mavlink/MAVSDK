cmake_minimum_required(VERSION 2.8.12)

#project(grpcdronecore)

add_definitions(
    -std=c++11
    -Wno-unused-parameter
    -Wno-expansion-to-defined
    -Wno-shadow
)

set(proto_dir ${CMAKE_SOURCE_DIR}/grpc/proto)

add_custom_command(OUTPUT dronecore.grpc.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --grpc_out=.
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        ${proto_dir}/dronecore.proto;
    DEPENDS ${proto_dir}/dronecore.proto
)

add_custom_command(OUTPUT dronecore.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --cpp_out=. ${proto_dir}/dronecore.proto
    DEPENDS ${proto_dir}/dronecore.proto
)

add_executable(dronecore_server
    dronecore_server.cpp
    dronecore.pb.cc # generated
    dronecore.grpc.pb.cc # generated
)

target_link_libraries(dronecore_server
    dronecore
    pthread
    ${Protobuf_PROTOC_LIBRARIES}
    ${GRPC_LIBRARIES}
    #grpc++_reflection
    dl
)

include_directories(
    ${Protobuf_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/grpc/server
)

add_definitions(-Wno-expansion-to-defined)

