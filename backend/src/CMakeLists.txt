cmake_minimum_required(VERSION 3.1)

set(COMPONENTS_LIST core action calibration camera mission telemetry info)

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
    grpc_server.cpp
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

add_dependencies(dronecode_sdk version)

target_link_libraries(backend
    dronecode_sdk
    dronecode_sdk_action
    dronecode_sdk_calibration
    dronecode_sdk_camera
    dronecode_sdk_mission
    dronecode_sdk_telemetry
    dronecode_sdk_info
    gRPC::grpc++
)

target_include_directories(backend
    PRIVATE
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/plugins
    ${CMAKE_BINARY_DIR}/backend/src
    ${PLUGINS_DIR}
)

if(IOS)
    set_target_properties(backend PROPERTIES
        FRAMEWORK TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_NAME_DIR @rpath
        PUBLIC_HEADER backend_api.h
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Dronecode SDK"
        MACOSX_FRAMEWORK_INFO_PLIST ${PROJECT_SOURCE_DIR}/backend/cmake/MacOSXFrameworkInfo.plist.in
    )
else()
    add_executable(backend_bin
        dronecode_sdk_server.cpp
    )

    target_link_libraries(backend_bin
        backend
        dronecode_sdk
    )

    target_include_directories(backend_bin
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/core
    )
endif()
