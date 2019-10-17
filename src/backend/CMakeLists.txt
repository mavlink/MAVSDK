cmake_minimum_required(VERSION 3.1)

find_package(OpenSSL REQUIRED)
find_package(gRPC REQUIRED)

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
