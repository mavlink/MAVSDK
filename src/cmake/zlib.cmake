set(ZLIB_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib)

set(SKIP_INSTALL_ALL "true")

include_directories("${ZLIB_ROOT_DIR}")
add_subdirectory(${ZLIB_ROOT_DIR} third_party/zlib)

if(IOS)
    set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE OFF)
else()
    set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

set(DRONECORE_ZLIB_LIBRARIES zlibstatic)
set(DRONECORE_ZLIB_INCLUDE_DIRS "${ZLIB_ROOT_DIR}" "${CMAKE_BINARY_DIR}/third_party/zlib")
