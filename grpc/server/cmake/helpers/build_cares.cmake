# Set c-ares variables
set(DRONECORESERVER_CARES_SOURCE_DIR "${DRONECORESERVER_EXTERNAL_SOURCE_DIR}/cares")
set(DRONECORESERVER_CARES_BINARY_DIR "${DRONECORESERVER_EXTERNAL_BINARY_DIR}/cares")

# Actually build c-ares
include(cmake/helpers/build_target.cmake)
build_target(${DRONECORESERVER_CARES_SOURCE_DIR} ${DRONECORESERVER_CARES_BINARY_DIR})

# Update CMAKE_PREFIX_PATH accordingly
list(APPEND CMAKE_PREFIX_PATH /cares/lib/cmake/c-ares)
