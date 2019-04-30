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

# For the build tree
configure_file(DronecodeSDKConfig.cmake.in
    "${PROJECT_BINARY_DIR}/DronecodeSDKConfig.cmake" @ONLY)
# For the install tree
configure_file(DronecodeSDKConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/DronecodeSDKConfig.cmake" @ONLY)

install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/DronecodeSDKConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DronecodeSDK)
