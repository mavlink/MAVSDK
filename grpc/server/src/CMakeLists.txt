cmake_minimum_required(VERSION 2.8)

file(STRINGS ${PLUGINS_DIR}/plugins.conf PLUGINS_LIST)

set(PROTOC_BINARY ${CMAKE_BINARY_DIR}/../default/third_party/protobuf/bin/protoc)
set(GRPC_CPP_PLUGIN_BINARY ${CMAKE_BINARY_DIR}/../default/third_party/grpc/bin/grpc_cpp_plugin)

if(NOT EXISTS ${PROTOC_BINARY} OR NOT EXISTS ${GRPC_CPP_PLUGIN_BINARY})
    message(FATAL_ERROR "Could not find 'protoc' or 'grpc_cpp_plugin' in the 'default' build folder. Please build for your host first (`make BUILD_DRONECORESERVER=YES default`).")
endif()

add_custom_command(OUTPUT core/core.grpc.pb.cc
    COMMAND ${PROTOC_BINARY}
        -I ${PROTO_DIR}
        --grpc_out=.
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_BINARY}
        ${PROTO_DIR}/core/core.proto;
)

add_custom_command(OUTPUT core/core.pb.cc
    COMMAND ${PROTOC_BINARY}
        -I ${PROTO_DIR}
        --cpp_out=.
        ${PROTO_DIR}/core/core.proto
)

foreach(PLUGIN ${PLUGINS_LIST})
    add_custom_command(OUTPUT ${PLUGIN}/${PLUGIN}.grpc.pb.cc
        COMMAND ${PROTOC_BINARY}
            -I ${PROTO_DIR}
            --grpc_out=.
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_BINARY}
            --cpp_out=.
            ${PROTO_DIR}/${PLUGIN}/${PLUGIN}.proto
    )
endforeach()

foreach(PLUGIN ${PLUGINS_LIST})
    add_custom_command(OUTPUT ${PLUGIN}/${PLUGIN}.pb.cc
        COMMAND ${PROTOC_BINARY}
            -I ${PROTO_DIR}
            --cpp_out=.
            ${PROTO_DIR}/${PLUGIN}/${PLUGIN}.proto
    )
endforeach()

set(PLUGINS_SRC dronecore_server.cpp core/core.pb.cc core/core.grpc.pb.cc)
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
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/plugins/action
    ${CMAKE_SOURCE_DIR}/plugins/mission
    ${CMAKE_SOURCE_DIR}/plugins/telemetry
    ${CMAKE_BINARY_DIR}/grpc/server/src
    ${PLUGINS_DIR}
)

target_link_libraries(
    dronecore_server
    dronecore
    dronecore_action
    dronecore_mission
    dronecore_telemetry
    gRPC::grpc++
    dl
)
