cmake_minimum_required(VERSION 3.13)

option(HUNTER_ENABLED "Enable Hunter package manager support" OFF)
option(SUPERBUILD "Build dependencies" ON)
option(BUILD_MAVSDK_SERVER "Build mavsdk_server" OFF)
option(BUILD_WITH_PROTO_REFLECTION "Build mavsdk_server with proto reflection" OFF)
option(BUILD_SHARED_LIBS "Build core as shared libraries instead of static ones" ON)
option(MAVLINK_DIALECT "MAVLink dialect. Default: common" "common")

if (SUPERBUILD AND HUNTER_ENABLED)
    message(FATAL_ERROR "Cannot SUPERBUILD while HUNTER_ENABLED. Set -DSUPERBUILD=OFF when using Hunter")
endif()

if (NOT MAVLINK_DIALECT)
    set(MAVLINK_DIALECT "common")
endif()

execute_process(
    COMMAND git describe --abbrev=8 --dirty --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GITVER
)

if (NOT GITVER EQUAL "0")
    message(STATUS "No git version found. Falling back to empty version no")
    set(VERSION_STR "0.0.0")
endif()

message(STATUS "Version: ${VERSION_STR}")
STRING(REGEX REPLACE v\([0-9]+\).[0-9]+.[0-9]+.*$ \\1 MAVSDK_VERSION_MAJOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.\([0-9]+\).[0-9]+.*$ \\1 MAVSDK_VERSION_MINOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.[0-9]+.\([0-9]+\).*$ \\1 MAVSDK_VERSION_PATCH "${VERSION_STR}")

message(STATUS "Version major: ${MAVSDK_VERSION_MAJOR}")
message(STATUS "Version minor: ${MAVSDK_VERSION_MINOR}")
message(STATUS "Version patch: ${MAVSDK_VERSION_PATCH}")

set(MAVSDK_SOVERSION_STRING ${MAVSDK_VERSION_MAJOR})
set(MAVSDK_VERSION_STRING ${MAVSDK_VERSION_MAJOR}.${MAVSDK_VERSION_MINOR}.${MAVSDK_VERSION_PATCH})

include("src/cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.23.315.tar.gz"
    SHA1 "ea5d72afc0df67f2126e7fd069f20c5f723709e1"
)

project(mavsdk_superbuild)

if (BUILD_BACKEND)
    message(FATAL_ERROR "The argument BUILD_BACKEND has been replaced by BUILD_MAVSDK_SERVER. To build mavsdk_server, use -DBUILD_MAVSDK_SERVER=ON.")
endif()

set(DEPS_BUILD_PATH "${PROJECT_BINARY_DIR}/third_party" CACHE PATH "Install path for the dependencies. Ignored if SUPERBUILD=OFF.")
set(DEPS_INSTALL_PATH "${DEPS_BUILD_PATH}/install" CACHE PATH "Install path for the dependencies. Ignored if SUPERBUILD=OFF.")

include(GNUInstallDirs)

add_subdirectory(third_party)
add_subdirectory(src)
