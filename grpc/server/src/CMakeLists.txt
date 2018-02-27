cmake_minimum_required(VERSION 3.1)

set(COMPONENTS_LIST core action mission telemetry)

include(cmake/compile_proto.cmake)

foreach(COMPONENT_NAME ${COMPONENTS_LIST})
    compile_proto_pb(${COMPONENT_NAME} PB_COMPILED_SOURCE)
    list(APPEND PB_COMPILED_SOURCES ${PB_COMPILED_SOURCE})

    compile_proto_grpc(${COMPONENT_NAME} GRPC_COMPILED_SOURCE)
    list(APPEND GRPC_COMPILED_SOURCES ${GRPC_COMPILED_SOURCE})
endforeach()

add_library(backend
    backend.cpp
    ${GRPC_COMPILED_SOURCES}
    ${PB_COMPILED_SOURCES}
)

target_link_libraries(backend
    dronecore
    dronecore_action
    dronecore_mission
    dronecore_telemetry
    gRPC::grpc++
)

target_include_directories(backend
    PRIVATE
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/plugins/action
    ${CMAKE_SOURCE_DIR}/plugins/mission
    ${CMAKE_SOURCE_DIR}/plugins/telemetry
    ${CMAKE_BINARY_DIR}/grpc/server/src
    ${PLUGINS_DIR}
)

add_executable(backend_bin
    dronecore_server.cpp
)

target_link_libraries(backend_bin
    backend
    dronecore
)

target_include_directories(backend_bin
    PRIVATE
    ${CMAKE_SOURCE_DIR}/core
)
