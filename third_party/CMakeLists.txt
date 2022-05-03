cmake_minimum_required(VERSION 3.10.2)

project(third_party)

include(cmake/build_target.cmake)

if (SUPERBUILD)
    list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

    build_target(jsoncpp)
    build_target(tinyxml2)

    if(NOT IOS)
        build_target(zlib)
    endif()

    build_target(curl)
    build_target(eventpp)

    if(BUILD_MAVSDK_SERVER)
        build_target(openssl)
        build_target(cares)
        build_target(protobuf)
        build_target(absl)
        build_target(re2)
        build_target(grpc)
    endif()
endif()

if(NOT MAVLINK_HEADERS)
    build_target(mavlink)
    set(MAVLINK_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/mavlink/mavlink/src/mavlink/include/")
    set(MAVLINK_HEADERS "${MAVLINK_HEADERS}" PARENT_SCOPE)
    message(STATUS "MAVLink headers for dialect ${MAVLINK_DIALECT} generated in ${MAVLINK_HEADERS}")
endif()
