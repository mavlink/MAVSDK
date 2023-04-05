cmake_minimum_required(VERSION 3.10.2)

project(third_party)

if (SUPERBUILD)
    list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

    add_subdirectory(jsoncpp)
    add_subdirectory(tinyxml2)

    if(NOT IOS)
        add_subdirectory(zlib)
    endif()

    add_subdirectory(curl)

    if(BUILD_MAVSDK_SERVER)
        add_subdirectory(openssl)
        add_subdirectory(cares)
        add_subdirectory(protobuf)
        add_subdirectory(absl)
        add_subdirectory(re2)
        add_subdirectory(grpc)
    endif()
endif()

if(NOT MAVLINK_HEADERS)
    add_subdirectory(mavlink)
    set(MAVLINK_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/install/include")
    set(MAVLINK_HEADERS ${MAVLINK_HEADERS} PARENT_SCOPE)
    message(STATUS "MAVLink headers for dialect ${MAVLINK_DIALECT} generated in ${MAVLINK_HEADERS}")
endif()
