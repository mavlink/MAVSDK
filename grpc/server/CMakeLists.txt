cmake_minimum_required(VERSION 2.8.12)
INCLUDE (FindPkgConfig)

project(rpcdronecore)

add_definitions("-std=c++11")
file(GLOB SRC_FILES
    "*.cc"
    "*.cpp"
)
add_executable(dronecore_server ${SRC_FILES})

include_directories("/usr/local/include")
include_directories("../../install/include")
ADD_LIBRARY(dronecore STATIC IMPORTED)
link_directories("/usr/local/lib")
SET_TARGET_PROPERTIES(dronecore PROPERTIES
    IMPORTED_LOCATION ../../../install/lib/libdronecore.a)
pkg_check_modules(GRPC grpc++)
target_link_libraries( dronecore_server ${GRPC_LIBRARIES} dronecore pthread protobuf grpc++_reflection dl)
