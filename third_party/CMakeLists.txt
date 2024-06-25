cmake_minimum_required(VERSION 3.10.2)

project(third_party)

add_subdirectory(mavlink)
add_subdirectory(jsoncpp)
add_subdirectory(tinyxml2)
add_subdirectory(liblzma)

if(NOT BUILD_WITHOUT_CURL)
    if(NOT IOS)
        add_subdirectory(zlib-ng)
    endif()
    add_subdirectory(curl)
endif()

if(BUILD_MAVSDK_SERVER)
    add_subdirectory(openssl)
    add_subdirectory(cares)
    add_subdirectory(absl)
    add_subdirectory(protobuf)
    add_subdirectory(re2)
    add_subdirectory(grpc)
endif()

add_subdirectory(gtest)

if (ENABLE_CPPTRACE)
    add_subdirectory(cpptrace)
endif()
