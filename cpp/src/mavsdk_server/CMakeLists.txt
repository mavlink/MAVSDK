cmake_minimum_required(VERSION 3.10.2)

find_package(gRPC REQUIRED)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
