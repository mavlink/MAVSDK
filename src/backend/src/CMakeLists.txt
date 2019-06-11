cmake_minimum_required(VERSION 3.1)

set(COMPONENTS_LIST core action calibration gimbal camera mission offboard telemetry info param)

include(cmake/compile_proto.cmake)

foreach(COMPONENT_NAME ${COMPONENTS_LIST})
    compile_proto_pb(${COMPONENT_NAME} PB_COMPILED_SOURCE)
    compile_proto_grpc(${COMPONENT_NAME} GRPC_COMPILED_SOURCE)
    add_library(${COMPONENT_NAME}_proto_gens ${GRPC_COMPILED_SOURCE} ${PB_COMPILED_SOURCE})
    target_link_libraries(${COMPONENT_NAME}_proto_gens
        gRPC::grpc++
    )
    target_include_directories(${COMPONENT_NAME}_proto_gens
      PRIVATE
      ${CMAKE_BINARY_DIR}/src/backend/src
    )
    list(APPEND COMPONENTS_PROTOGENS ${COMPONENT_NAME}_proto_gens)
endforeach()

set(BACKEND_SOURCES
    backend_api.h
    backend_api.cpp
    backend.cpp
    grpc_server.cpp
)

if(IOS)
    set_property(SOURCE module.modulemap
        PROPERTY MACOSX_PACKAGE_LOCATION "Modules")

    list(APPEND BACKEND_SOURCES module.modulemap)
endif()

if(IOS)
    add_library(backend SHARED ${BACKEND_SOURCES})
else()
    add_library(backend ${BACKEND_SOURCES})
endif()

target_link_libraries(backend
    dronecode_sdk_action
    dronecode_sdk_calibration
    dronecode_sdk_gimbal
    dronecode_sdk_camera
    dronecode_sdk_mission
    dronecode_sdk_offboard
    dronecode_sdk_telemetry
    dronecode_sdk_info
    dronecode_sdk_param
    dronecode_sdk
    gRPC::grpc++
    ${COMPONENTS_PROTOGENS}
)

target_include_directories(backend
    PRIVATE
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/plugins
    ${PROJECT_SOURCE_DIR}/backend/src/plugins
    ${CMAKE_BINARY_DIR}/src/backend/src
)

if(IOS)
    set_target_properties(backend PROPERTIES
        FRAMEWORK TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_NAME_DIR @rpath
        PUBLIC_HEADER backend_api.h
        #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
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
