cmake_minimum_required(VERSION 2.8.12)

#project(grpcdronecore)


add_definitions(
    -std=c++11
    -Wno-unused-parameter
    -Wno-shadow
)

set(proto_dir ${CMAKE_SOURCE_DIR}/grpc/proto)
set(plugins_dir ${CMAKE_SOURCE_DIR}/grpc/server/plugins/)
set(plugins_list action mission telemetry)

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


foreach(plugin ${plugins_list})
    add_custom_command(OUTPUT ${plugin}.grpc.pb.cc
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            -I ${proto_dir}
            -I ${plugins_dir}/${plugin}
            --grpc_out=.
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
            --cpp_out=. ${plugins_dir}/${plugin}/${plugin}.proto
        DEPENDS ${plugins_dir}/${plugin}/${plugin}.proto
)
endforeach()

foreach(plugin ${plugins_list})
    add_custom_command(OUTPUT ${plugin}.pb.cc
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            -I ${proto_dir}
            -I ${plugins_dir}/${plugin}
            --cpp_out=. ${plugins_dir}/${plugin}/${plugin}.proto
        DEPENDS ${plugins_dir}/${plugin}/${plugin}.proto
)
endforeach()

set(plugins_src ${CMAKE_CURRENT_BINARY_DIR}/dronecore_server.cpp dronecore.pb.cc dronecore.grpc.pb.cc)
foreach(plugin ${plugins_list})
    list(APPEND plugins_src ${plugin}.pb.cc)
    list(APPEND plugins_src ${plugin}.grpc.pb.cc)
endforeach(plugin)

add_executable(dronecore_server ${plugins_src})

target_link_libraries(dronecore_server
    dronecore
    ${Protobuf_PROTOC_LIBRARIES}
    ${GRPC_LIBRARIES}
    #grpc++_reflection
    dl
)

include(autogenerate_plugin_container.cmake)

include_directories(
    ${Protobuf_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/grpc/server
    ${plugins_dir}
)
