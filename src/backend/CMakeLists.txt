cmake_minimum_required(VERSION 3.1)

find_package(OpenSSL REQUIRED)
find_package(gRPC REQUIRED)
if (NOT BUILD_SUPERBUILD)
    find_package(Protobuf REQUIRED)
endif()

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
