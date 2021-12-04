cmake_minimum_required(VERSION 3.13)

option(HUNTER_ENABLED "Enable Hunter package manager support" OFF)
option(SUPERBUILD "Build dependencies" ON)
option(BUILD_MAVSDK_SERVER "Build mavsdk_server" OFF)
option(BUILD_SHARED_LIBS "Build core as shared libraries instead of static ones" ON)

if(SUPERBUILD AND HUNTER_ENABLED)
    message(FATAL_ERROR "Cannot SUPERBUILD while HUNTER_ENABLED. Set -DSUPERBUILD=OFF when using Hunter")
endif()

include("src/cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.315.tar.gz"
    SHA1 "ea5d72afc0df67f2126e7fd069f20c5f723709e1"
)

execute_process(
    COMMAND git describe --abbrev=8 --dirty --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Version: ${VERSION_STR}")
STRING(REGEX REPLACE v\([0-9]+\).[0-9]+.[0-9]+.*$ \\1 MAVSDK_VERSION_MAJOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.\([0-9]+\).[0-9]+.*$ \\1 MAVSDK_VERSION_MINOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.[0-9]+.\([0-9]+\).*$ \\1 MAVSDK_VERSION_PATCH "${VERSION_STR}")

message(STATUS "Version major: ${MAVSDK_VERSION_MAJOR}")
message(STATUS "Version minor: ${MAVSDK_VERSION_MINOR}")
message(STATUS "Version patch: ${MAVSDK_VERSION_PATCH}")

set(MAVSDK_SOVERSION_STRING ${MAVSDK_VERSION_MAJOR})
set(MAVSDK_VERSION_STRING ${MAVSDK_VERSION_MAJOR}.${MAVSDK_VERSION_MINOR}.${MAVSDK_VERSION_PATCH})

project(mavsdk_superbuild)

if (BUILD_BACKEND)
    message(FATAL_ERROR "The argument BUILD_BACKEND has been replaced by BUILD_MAVSDK_SERVER. To build mavsdk_server, use -DBUILD_MAVSDK_SERVER=ON.")
endif()

set(DEPS_BUILD_PATH "${PROJECT_BINARY_DIR}/third_party" CACHE PATH "Install path for the dependencies. Ignored if SUPERBUILD=OFF.")
set(DEPS_INSTALL_PATH "${DEPS_BUILD_PATH}/install" CACHE PATH "Install path for the dependencies. Ignored if SUPERBUILD=OFF.")

include(GNUInstallDirs)

if(SUPERBUILD)
    add_subdirectory(third_party)
endif()

add_subdirectory(src)

install(EXPORT mavsdk-targets
    FILE MAVSDKTargets.cmake
    NAMESPACE MAVSDK::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MAVSDK
    )

# For the build tree
configure_file(MAVSDKConfig.cmake.in
    "${PROJECT_BINARY_DIR}/MAVSDKConfig.cmake" @ONLY)
# For the install tree
configure_file(MAVSDKConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MAVSDKConfig.cmake" @ONLY)

include(CMakePackageConfigHelpers)
# Supply version to config
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MAVSDKConfigVersion.cmake
    VERSION ${MAVSDK_VERSION_MAJOR}.${MAVSDK_VERSION_MINOR}.${MAVSDK_VERSION_PATCH}
    COMPATIBILITY SameMajorVersion )

install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MAVSDKConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MAVSDKConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MAVSDK)
