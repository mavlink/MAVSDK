#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CURL::libcurl" for configuration "Debug"
set_property(TARGET CURL::libcurl APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CURL::libcurl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libcurl-d.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS CURL::libcurl )
list(APPEND _IMPORT_CHECK_FILES_FOR_CURL::libcurl "${_IMPORT_PREFIX}/lib/libcurl-d.a" )

# Import target "CURL::curl" for configuration "Debug"
set_property(TARGET CURL::curl APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(CURL::curl PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/curl"
  )

list(APPEND _IMPORT_CHECK_TARGETS CURL::curl )
list(APPEND _IMPORT_CHECK_FILES_FOR_CURL::curl "${_IMPORT_PREFIX}/bin/curl" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
