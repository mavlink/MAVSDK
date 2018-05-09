# We need tinyxml2 for the camera definition parsing.

# We save the value of BUILD_STATIC_LIBRARY and BUILD_SHARED_LIBRARY before altering them
set(BEFORE_TINYXML_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BEFORE_TINYXML_BUILD_STATIC_LIBS ${BUILD_STATIC_LIBS})
# This trick is needed with the cache as it is an option of tinyxml2,
# and tinyxml2 is used as a submodule
set(BUILD_SHARED_LIBS CACHE BOOL OFF)
set(BUILD_STATIC_LIBS CACHE BOOL ON)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

# Being linked to a shared lib later, tinyxml2_static needs to be compiled with -fPIC
# Same trick here, we don't want to set -fPIC outside of this file
set(BEFORE_TINYXML_POSITION_INDEPENDENT_CODE ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Actually include tinyxml2
add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
link_directories(third_party/tinyxml2)
include_directories(SYSTEM third_party/tinyxml2)

set(TINYXML2_LIBRARY tinyxml2_static)

# We set CMAKE_POSITION_INDEPENDENT_CODE back to what it was before including this file
set(CMAKE_POSITION_INDEPENDENT_CODE ${BEFORE_TINYXML_POSITION_INDEPENDENT_CODE})

# We set BUILD_STATIC_LIBRARY back to what it was before including this file
if (DEFINED BEFORE_TINYXML_BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS CACHE BOOL ${BEFORE_TINYXML_BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ${BEFORE_TINYXML_BUILD_SHARED_LIBS})
else()
    unset(BUILD_SHARED_LIBS CACHE)
    unset(BUILD_SHARED_LIBS)
endif()

if (DEFINED BEFORE_TINYXML_BUILD_STATIC_LIBS)
    set(BUILD_STATIC_LIBS CACHE BOOL ${BEFORE_TINYXML_BUILD_STATIC_LIBS})
    set(BUILD_STATIC_LIBS ${BEFORE_TINYXML_BUILD_STATIC_LIBS})
else()
    unset(BUILD_STATIC_LIBS CACHE)
    unset(BUILD_STATIC_LIBS)
endif()
