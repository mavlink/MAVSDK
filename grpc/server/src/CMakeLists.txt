cmake_minimum_required(VERSION 3.1)

set(COMPONENTS_LIST core action mission telemetry)

include(cmake/compile_proto.cmake)

foreach(COMPONENT_NAME ${COMPONENTS_LIST})
    compile_proto_pb(${COMPONENT_NAME} PB_COMPILED_SOURCE)
    list(APPEND PB_COMPILED_SOURCES ${PB_COMPILED_SOURCE})

    compile_proto_grpc(${COMPONENT_NAME} GRPC_COMPILED_SOURCE)
    list(APPEND GRPC_COMPILED_SOURCES ${GRPC_COMPILED_SOURCE})
endforeach()

set(BACKEND_SOURCES
    backend_api.h
    backend_api.cpp
    backend.cpp
    ${GRPC_COMPILED_SOURCES}
    ${PB_COMPILED_SOURCES}
)

if(IOS)
    set_property(SOURCE module.modulemap
        PROPERTY MACOSX_PACKAGE_LOCATION "Modules")

    list(APPEND BACKEND_SOURCES module.modulemap)
endif()

if(ANDROID OR IOS)
    add_library(backend SHARED ${BACKEND_SOURCES})
else()
    add_library(backend STATIC ${BACKEND_SOURCES})
endif()

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

if(IOS)
    set_target_properties(backend PROPERTIES
        FRAMEWORK TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_NAME_DIR @rpath
        MACOSX_FRAMEWORK_IDENTIFIER io.dronecore.backend
        VERSION 0.0.1
        SOVERSION 0.0.1
        PUBLIC_HEADER backend_api.h
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "DroneCore"
    )
else()
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
endif()
