# Set protobuf variables
set(DRONECORESERVER_PROTOBUF_SOURCE_DIR "${DRONECORESERVER_EXTERNAL_SOURCE_DIR}/protobuf")
set(DRONECORESERVER_PROTOBUF_BINARY_DIR "${DRONECORESERVER_EXTERNAL_BINARY_DIR}/protobuf")

# Actually build protobuf
include(cmake/helpers/build_target.cmake)
build_target(${DRONECORESERVER_PROTOBUF_SOURCE_DIR} ${DRONECORESERVER_PROTOBUF_BINARY_DIR})

# Update CMAKE_PREFIX_PATH accordingly
list(APPEND CMAKE_PREFIX_PATH /protobuf/lib/cmake/protobuf)
