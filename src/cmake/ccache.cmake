# ccache
option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE AND CCACHE_PROGRAM AND NOT DEFINED ENV{CCACHE_DISABLE})

    get_filename_component(ccache_real_path ${CCACHE_PROGRAM} REALPATH)
    get_filename_component(cxx_real_path ${CMAKE_CXX_COMPILER} REALPATH)
    get_filename_component(cxx_abs_path ${CMAKE_CXX_COMPILER} ABSOLUTE)

    if ("${ccache_real_path}" STREQUAL "${cxx_real_path}")
        message(STATUS "ccache enabled via symlink (${cxx_abs_path} -> ${cxx_real_path})")
    else()
        message(STATUS "ccache enabled (export CCACHE_DISABLE=1 to disable)")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()
endif()
