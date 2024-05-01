cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0025 NEW)

project(mavsdk)

# ccache
option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE AND CCACHE_PROGRAM AND NOT DEFINED ENV{CCACHE_DISABLE})

    get_filename_component(ccache_real_path ${CCACHE_PROGRAM} REALPATH)
    get_filename_component(cxx_real_path ${CMAKE_CXX_COMPILER} REALPATH)
    get_filename_component(cxx_abs_path ${CMAKE_CXX_COMPILER} ABSOLUTE)

    if ("${ccache_real_path}" STREQUAL "${cxx_real_path}")
        message(STATUS "ccache enabled via symlink (${cxx_abs_path} -> ${cxx_real_path})")
    else()
        message(STATUS "ccache enabled (export CCACHE_DISABLE=1 to disable)")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()
endif()

option(BUILD_TESTS "Build tests" ON)
option(CMAKE_POSITION_INDEPENDENT_CODE "Position independent code" ON)

include(cmake/compiler_flags.cmake)

find_package(Threads REQUIRED)

if(NOT HUNTER_ENABLED)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
endif()

# This will noop if hunter is not enabled
hunter_add_package(jsoncpp)
hunter_add_package(CURL)
hunter_add_package(lzma)

find_package(CURL REQUIRED)
find_package(LibLZMA REQUIRED)

if(BUILD_TESTS AND (IOS OR ANDROID))
    message(STATUS "Building for iOS or Android: forcing BUILD_TESTS to FALSE...")
    set(BUILD_TESTS OFF)
endif()

if(ANDROID)
    set(lib_path "lib/android/${ANDROID_ABI}")
elseif(IOS)
    set(lib_path "lib/ios")
else()
    set(lib_path "lib")
endif()

if (ENABLED_PLUGINS)
    message(STATUS "Manually enabled plugins:")
    foreach(plugin ${ENABLED_PLUGINS})
        message(STATUS "  - ${plugin}")
    endforeach()
else()
    message(STATUS "All plugins enabled")
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/plugins.txt" ENABLED_PLUGINS)
endif()

add_subdirectory(mavsdk)

include(cmake/static_analyzers.cmake)

if(BUILD_TESTS)
    enable_testing()

    add_subdirectory(integration_tests)
    add_subdirectory(unit_tests)
    add_subdirectory(system_tests)
endif()

if (BUILD_MAVSDK_SERVER)
    message(STATUS "Building mavsdk server")
    add_subdirectory(mavsdk_server)
else()
    message(STATUS "BUILD_MAVSDK_SERVER not set: not building grpc mavsdk_server")
endif()

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

# Pkg-config
configure_file(mavsdk.pc.in
    "${PROJECT_BINARY_DIR}/mavsdk.pc" @ONLY)

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

install(FILES
    "${PROJECT_BINARY_DIR}/mavsdk.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
