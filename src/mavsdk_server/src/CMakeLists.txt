cmake_minimum_required(VERSION 3.10.2)

list(APPEND MAVSDK_SERVER_SOURCES
    mavsdk_server_api.h
    mavsdk_server_api.cpp
    mavsdk_server.cpp
    grpc_server.cpp
)

set(COMPONENTS_LIST ${ENABLED_PLUGINS})
list(APPEND COMPONENTS_LIST core)
foreach(COMPONENT_NAME ${COMPONENTS_LIST})
    list(APPEND MAVSDK_SERVER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/${COMPONENT_NAME}/${COMPONENT_NAME}.grpc.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/${COMPONENT_NAME}/${COMPONENT_NAME}.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/mavsdk_options.grpc.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/mavsdk_options.pb.cc
        )
endforeach()

if(IOS OR (APPLE AND MACOS_FRAMEWORK))
    set_property(SOURCE module.modulemap
        PROPERTY MACOSX_PACKAGE_LOCATION "Modules")

    list(APPEND MAVSDK_SERVER_SOURCES module.modulemap)
endif()

if(IOS OR ANDROID OR (APPLE AND MACOS_FRAMEWORK))
    add_library(mavsdk_server SHARED ${MAVSDK_SERVER_SOURCES})
else()
    add_library(mavsdk_server ${MAVSDK_SERVER_SOURCES})
endif()

target_link_libraries(mavsdk_server
    PRIVATE
    mavsdk
    gRPC::grpc++
)

if(BUILD_WITH_PROTO_REFLECTION)
    add_definitions(-DENABLE_PROTO_REFLECTION)
    target_link_libraries(mavsdk_server PRIVATE gRPC::grpc++_reflection)
endif()

foreach(plugin ${COMPONENTS_LIST})
    string(TOUPPER ${plugin} plugin_uppercase)
    target_compile_options(mavsdk_server
        PRIVATE
        "-D${plugin_uppercase}_ENABLED"
    )
endforeach()


set_target_properties(mavsdk_server
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
    )

if(BUILD_STATIC_MAVSDK_SERVER AND ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
    target_link_libraries(mavsdk_server PRIVATE atomic.a)
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "(armv6|armv7)")
    target_link_libraries(mavsdk_server PRIVATE atomic)
endif()

target_include_directories(mavsdk_server
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/mavsdk/core>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/mavsdk/plugins>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/mavsdk_server/src/plugins>
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/mavsdk_server/src/generated>
    $<INSTALL_INTERFACE:include>
)

# Build and install mavsdk_server for all but iOS and Android
if(NOT IOS AND NOT ANDROID)
    add_executable(mavsdk_server_bin
        mavsdk_server_bin.cpp
    )

    target_link_libraries(mavsdk_server_bin PRIVATE
        mavsdk_server
        mavsdk
    )

    if (BUILD_STATIC_MAVSDK_SERVER)
        target_link_options(mavsdk_server_bin PRIVATE -static-libgcc -static-libstdc++ -static)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        set_target_properties(mavsdk_server_bin PROPERTIES LINK_SEARCH_START_STATIC ON)
        set_target_properties(mavsdk_server_bin PROPERTIES LINK_SEARCH_END_STATIC ON)
    endif()

    if(NOT BUILD_STATIC_MAVSDK_SERVER AND ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
        target_link_libraries(mavsdk_server_bin PRIVATE ${CMAKE_DL_LIBS})
    endif()

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
endif()

# iOS builds mavsdk_server.framework
if(IOS OR (APPLE AND MACOS_FRAMEWORK))
    set_target_properties(mavsdk_server PROPERTIES
        FRAMEWORK TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_NAME_DIR @rpath
        PUBLIC_HEADER mavsdk_server_api.h
        #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
        MACOSX_FRAMEWORK_INFO_PLIST ${PROJECT_SOURCE_DIR}/mavsdk_server/cmake/MacOSXFrameworkInfo.plist.in
    )
endif()

if(NOT IOS)
    # Install libmavsdk_server when built as a shared lib. This happens when
    # BUILD_SHARED_LIBS is set to 'ON', and it always happens on ANDROID.
    if(ANDROID OR BUILD_SHARED_LIBS)
        install(TARGETS mavsdk_server
            EXPORT mavsdk-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

        install(FILES
            mavsdk_server_api.h
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/mavsdk_server"
        )
    endif()
endif()

