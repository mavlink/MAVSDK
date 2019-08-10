cmake_minimum_required(VERSION 3.1)

set(COMPONENTS_LIST core action calibration gimbal camera mission offboard telemetry info param)

include(cmake/compile_proto.cmake)

foreach(COMPONENT_NAME ${COMPONENTS_LIST})
    compile_proto_pb(${COMPONENT_NAME} PB_COMPILED_SOURCE)
    compile_proto_grpc(${COMPONENT_NAME} GRPC_COMPILED_SOURCE)
    add_library(${COMPONENT_NAME}_proto_gens STATIC ${GRPC_COMPILED_SOURCE} ${PB_COMPILED_SOURCE})
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
    add_library(mavsdk_server SHARED ${BACKEND_SOURCES})
else()
    add_library(mavsdk_server ${BACKEND_SOURCES})
endif()

target_link_libraries(mavsdk_server
    PRIVATE
    mavsdk_action
    mavsdk_calibration
    mavsdk_gimbal
    mavsdk_camera
    mavsdk_mission
    mavsdk_offboard
    mavsdk_telemetry
    mavsdk_info
    mavsdk_param
    mavsdk
    gRPC::grpc++
    ${COMPONENTS_PROTOGENS}
)

target_include_directories(mavsdk_server
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/plugins>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/backend/src/plugins>
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/backend/src>
    $<INSTALL_INTERFACE:include>
)

if(IOS)
    set_target_properties(mavsdk_server PROPERTIES
        FRAMEWORK TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_NAME_DIR @rpath
        PUBLIC_HEADER backend_api.h
        #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        MACOSX_FRAMEWORK_INFO_PLIST ${PROJECT_SOURCE_DIR}/backend/cmake/MacOSXFrameworkInfo.plist.in
    )
else()
    add_executable(mavsdk_server_bin
        mavsdk_server.cpp
    )

    target_link_libraries(mavsdk_server_bin
        mavsdk_server
        mavsdk
    )

    # MSVC fails to generate the `mavsdk_server` binary while having
    # a library called `mavsdk_server` as well. This means that with
    # MSVC, we build `mavsdk_server_bin.exe`.
    if(NOT MSVC)
        set_target_properties(mavsdk_server_bin PROPERTIES
            OUTPUT_NAME mavsdk_server
        )
    endif()

    install(TARGETS mavsdk_server_bin
        EXPORT mavsdk-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(BUILD_SHARED_LIBS)
        install(TARGETS mavsdk_server
            EXPORT mavsdk-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

        install(FILES
            backend_api.h
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/backend"
        )
    endif()
endif()
