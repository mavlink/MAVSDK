cmake_minimum_required(VERSION 2.8)

file(STRINGS ${PLUGINS_DIR}/plugins.conf PLUGINS_LIST)

add_custom_command(OUTPUT dronecore/dronecore.grpc.pb.cc
    COMMAND $<TARGET_FILE:protobuf::protoc>
        -I ${PROTO_DIR}
        --grpc_out=.
        --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
        ${PROTO_DIR}/dronecore/dronecore.proto;
        #DEPENDS ${PROTO_DIR}/dronecore.proto
)

add_custom_command(OUTPUT dronecore/dronecore.pb.cc
    COMMAND $<TARGET_FILE:protobuf::protoc>
        -I ${PROTO_DIR}
        --cpp_out=.
        ${PROTO_DIR}/dronecore/dronecore.proto
        #DEPENDS ${PROTO_DIR}/dronecore.proto
)

foreach(PLUGIN ${PLUGINS_LIST})
    add_custom_command(OUTPUT ${PLUGIN}/${PLUGIN}.grpc.pb.cc
        COMMAND $<TARGET_FILE:protobuf::protoc>
            -I ${PROTO_DIR}
            --grpc_out=.
            --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
            --cpp_out=.
            ${PROTO_DIR}/${PLUGIN}/${PLUGIN}.proto
            #DEPENDS ${PROTO_DIR}/${PLUGIN}.proto
    )
endforeach()

foreach(PLUGIN ${PLUGINS_LIST})
    add_custom_command(OUTPUT ${PLUGIN}/${PLUGIN}.pb.cc
        COMMAND $<TARGET_FILE:protobuf::protoc>
            -I ${PROTO_DIR}
            --cpp_out=.
            ${PROTO_DIR}/${PLUGIN}/${PLUGIN}.proto
            #DEPENDS ${PROTO_DIR}/${PLUGIN}.proto
    )
endforeach()

set(PLUGINS_SRC dronecore_server.cpp dronecore/dronecore.pb.cc dronecore/dronecore.grpc.pb.cc)
foreach(PLUGIN ${PLUGINS_LIST})
    list(APPEND PLUGINS_SRC ${PLUGIN}/${PLUGIN}.pb.cc)
    list(APPEND PLUGINS_SRC ${PLUGIN}/${PLUGIN}.grpc.pb.cc)
endforeach()

add_executable(dronecore_server ${PLUGINS_SRC})

target_compile_options(
    dronecore_server
    PUBLIC
    -Wno-unused-parameter
    -Wno-shadow
)

target_include_directories(
    dronecore_server
    PUBLIC
    ${CMAKE_BINARY_DIR}/grpc/server/src
    ${PLUGINS_DIR}
)

target_link_libraries(
    dronecore_server
    dronecore
    gRPC::grpc++
    dl
)
