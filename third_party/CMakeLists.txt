cmake_minimum_required(VERSION 3.10.2)

project(third_party)

include(cmake/build_target.cmake)

if (SUPERBUILD)
    list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

    build_target(mavlink)
    build_target(jsoncpp)
    build_target(tinyxml2)
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

    build_target(gtest)

endif()

if (ENABLE_CPPTRACE)
    list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
    build_target(cpptrace)
endif()
