cmake_minimum_required(VERSION 2.8.12)

project(dronecore-server)

set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PLUGINS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins)

include(GNUInstallDirs)
include(cmake/helpers/build_external.cmake)

add_subdirectory(src)
