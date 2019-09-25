# Copied from https://github.com/ros2/tinyxml2_vendor/blob/master/cmake/Modules/FindTinyXML2.cmake

# TinyXML2_FOUND
# TinyXML2_INCLUDE_DIRS
# TinyXML2_LIBRARIES

# try to find the CMake config file for TinyXML2 first
find_package(TinyXML2 CONFIG QUIET)
if(TinyXML2_FOUND)
  message(STATUS "Found TinyXML2 via Config file: ${TinyXML2_DIR}")
  if(NOT TINYXML2_LIBRARY)
    # in this case, we're probably using TinyXML2 version 5.0.0 or greater
    # in which case tinyxml2 is an exported target and we should use that
    if(TARGET tinyxml2)
      set(TINYXML2_LIBRARY tinyxml2)
    elseif(TARGET tinyxml2::tinyxml2)
      set(TINYXML2_LIBRARY tinyxml2::tinyxml2)
    endif()
  endif()
else()
  find_path(TINYXML2_INCLUDE_DIR NAMES tinyxml2.h)

  find_library(TINYXML2_LIBRARY tinyxml2)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(TinyXML2 DEFAULT_MSG TINYXML2_LIBRARY TINYXML2_INCLUDE_DIR)

  mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY)
endif()

# Set mixed case INCLUDE_DIRS and LIBRARY variables from upper case ones.
if(NOT TinyXML2_INCLUDE_DIRS)
  set(TinyXML2_INCLUDE_DIRS ${TINYXML2_INCLUDE_DIR})
endif()
if(NOT TinyXML2_LIBRARIES)
  set(TinyXML2_LIBRARIES ${TINYXML2_LIBRARY})
endif()
