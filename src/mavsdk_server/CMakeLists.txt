cmake_minimum_required(VERSION 3.10.2)

hunter_add_package(gRPC)
find_package(OpenSSL REQUIRED)
find_package(gRPC REQUIRED)

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
