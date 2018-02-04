set(ZLIB_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/zlib)

include_directories("${ZLIB_ROOT_DIR}")
add_subdirectory(${ZLIB_ROOT_DIR} third_party/zlib)

if(NOT ANDROID AND NOT IOS)
    set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE ON)
else()
    set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE OFF)
endif()

set(DRONECORE_ZLIB_LIBRARIES zlibstatic)
set(DRONECORE_ZLIB_INCLUDE_DIRS "${ZLIB_ROOT_DIR}" "${CMAKE_BINARY_DIR}/third_party/zlib")
