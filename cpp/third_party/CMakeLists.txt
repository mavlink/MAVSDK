cmake_minimum_required(VERSION 3.10.2)

project(third_party)
include(FetchContent)

set(DEPS_INSTALL_PATH "${PROJECT_BINARY_DIR}/install" CACHE PATH "Install path for the dependencies.")

list(APPEND CMAKE_PREFIX_PATH "${DEPS_INSTALL_PATH}")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

if (CMAKE_CROSSCOMPILING)
    list(APPEND CMAKE_FIND_ROOT_PATH "${DEPS_INSTALL_PATH}")
    set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} PARENT_SCOPE)
endif()

FetchContent_Declare(
    picosha2
    GIT_REPOSITORY https://github.com/julianoes/PicoSHA2
    GIT_TAG cmake-install-support
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(picosha2)

add_library(picosha2::picosha2 INTERFACE IMPORTED)
set_target_properties(picosha2::picosha2 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${picosha2_SOURCE_DIR}"
)

add_subdirectory(libmavlike)
add_subdirectory(libevents)
add_subdirectory(mavlink)