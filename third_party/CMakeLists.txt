cmake_minimum_required(VERSION 3.1)

project(third_party)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "CMake install prefix path for dependencies" FORCE)
endif()

include(cmake/build_target.cmake)

set(THIRD_PARTY_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(THIRD_PARTY_BINARY_DIR "${THIRD_PARTY_SOURCE_DIR}/build")
set(THIRD_PARTY_INSTALL_DIR "${THIRD_PARTY_BINARY_DIR}/install")

list(APPEND CMAKE_PREFIX_PATH "${THIRD_PARTY_INSTALL_DIR}")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)

build_target(${THIRD_PARTY_SOURCE_DIR}/tinyxml2 ${THIRD_PARTY_BINARY_DIR}/tinyxml2 ${THIRD_PARTY_INSTALL_DIR})
build_target(${THIRD_PARTY_SOURCE_DIR}/zlib ${THIRD_PARTY_BINARY_DIR}/zlib ${THIRD_PARTY_INSTALL_DIR})

# In SUPERBUILD, we want to link dependencies statically. Because zlib doesn't
# allow to disable building the shared libs, we remove them manually.
FILE(GLOB_RECURSE LIBZ_SHARED_LIBS "${THIRD_PARTY_INSTALL_DIR}/*libz.so*")
foreach(LIBZ_SHARED_LIB ${LIBZ_SHARED_LIBS})
    FILE(REMOVE "${LIBZ_SHARED_LIB}")
endforeach()

build_target(${THIRD_PARTY_SOURCE_DIR}/curl ${THIRD_PARTY_BINARY_DIR}/curl ${THIRD_PARTY_INSTALL_DIR})

if(${BUILD_BACKEND})
    build_target(${THIRD_PARTY_SOURCE_DIR}/libressl ${THIRD_PARTY_BINARY_DIR}/libressl ${THIRD_PARTY_INSTALL_DIR})
    build_target(${THIRD_PARTY_SOURCE_DIR}/cares ${THIRD_PARTY_BINARY_DIR}/cares ${THIRD_PARTY_INSTALL_DIR})
    build_target(${THIRD_PARTY_SOURCE_DIR}/protobuf ${THIRD_PARTY_BINARY_DIR}/protobuf ${THIRD_PARTY_INSTALL_DIR})
    build_target(${THIRD_PARTY_SOURCE_DIR}/grpc ${THIRD_PARTY_BINARY_DIR}/grpc ${THIRD_PARTY_INSTALL_DIR})
endif()
