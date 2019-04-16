cmake_minimum_required(VERSION 3.1)

project(dronecode_sdk_superbuild)

option(SUPERBUILD "Build dependencies" ON)
option(BUILD_BACKEND "Build backend" OFF)

set(CMAKE_DEBUG_POSTFIX "")

if(SUPERBUILD)
    add_subdirectory(third_party)
endif()

add_subdirectory(src)
