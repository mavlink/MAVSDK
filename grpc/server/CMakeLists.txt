cmake_minimum_required(VERSION 2.8.12)

#project(grpcdronecore)


add_definitions(
    -std=c++11
    -Wno-unused-parameter
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

add_custom_command(OUTPUT action.grpc.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --grpc_out=.
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        --cpp_out=. ${proto_dir}/action.proto
    DEPENDS ${proto_dir}/action.proto
)

add_custom_command(OUTPUT action.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --cpp_out=. ${proto_dir}/action.proto
    DEPENDS ${proto_dir}/action.proto
)

add_custom_command(OUTPUT mission.grpc.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --grpc_out=.
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        --cpp_out=. ${proto_dir}/mission.proto
    DEPENDS ${proto_dir}/mission.proto
)

add_custom_command(OUTPUT mission.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --cpp_out=. ${proto_dir}/mission.proto
    DEPENDS ${proto_dir}/mission.proto
)

add_custom_command(OUTPUT telemetry.grpc.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --grpc_out=.
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        --cpp_out=. ${proto_dir}/telemetry.proto
    DEPENDS ${proto_dir}/telemetry.proto
)

add_custom_command(OUTPUT telemetry.pb.cc
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        -I ${proto_dir}
        --cpp_out=. ${proto_dir}/telemetry.proto
    DEPENDS ${proto_dir}/telemetry.proto
)

add_executable(dronecore_server
    dronecore_server.cpp
    dronecore.pb.cc # generated
    dronecore.grpc.pb.cc # generated
	action.pb.cc # generated
    action.grpc.pb.cc # generated
    mission.pb.cc # generated
    mission.grpc.pb.cc # generated
    telemetry.pb.cc # generated
    telemetry.grpc.pb.cc # generated
)

target_link_libraries(dronecore_server
    dronecore
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
