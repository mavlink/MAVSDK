cmake_minimum_required(VERSION 3.10.2)

project(third_party)

include(cmake/build_target.cmake)

list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

if (CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_FIND_ROOT_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} PARENT_SCOPE)
endif()

if(SUPERBUILD)
    set(MAVLINK_HASH "d6a7eeaf43319ce6da19a1973ca40180a4210643"
        CACHE STRING "Hash for the MAVLink Repository")
    set(MAVLINK_URL "https://github.com/mavlink/mavlink"
        CACHE STRING "URL for the MAVLink Repository")

    build_target(mavlink)
    build_target(libevents)
    build_target(picosha2)
    build_target(tinyxml2)
    build_target(libmavlike)
    build_target(jsoncpp)
    build_target(liblzma)

    if(NOT BUILD_WITHOUT_CURL)
        if(ANDROID)
            build_target(boringssl)
        else()
            build_target(openssl)
        endif()

        if(NOT IOS)
            build_target(zlib-ng)
        endif()

        build_target(curl)
    endif()

    if(BUILD_MAVSDK_SERVER)
        if (BUILD_WITHOUT_CURL)
            if(ANDROID)
                build_target(boringssl)
            else()
                build_target(openssl)
            endif()
        endif()

        build_target(cares)
        build_target(absl)
        build_target(protobuf)
        build_target(re2)
        build_target(grpc)
    endif()

    if(BUILD_TESTING)
        build_target(gtest)
    endif()

endif()

if(ENABLE_CPPTRACE)
    build_target(cpptrace)
endif()
