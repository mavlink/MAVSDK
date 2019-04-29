cmake_minimum_required(VERSION 3.1)

project(dronecode_sdk_superbuild)

option(SUPERBUILD "Build dependencies" ON)
option(BUILD_BACKEND "Build backend" OFF)

include(GNUInstallDirs)

if(SUPERBUILD)
    add_subdirectory(third_party)
endif()

add_subdirectory(src)

install(EXPORT dronecode-sdk-targets
    FILE DronecodeSDKTargets.cmake
    NAMESPACE DronecodeSDK::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DronecodeSDK
    )
