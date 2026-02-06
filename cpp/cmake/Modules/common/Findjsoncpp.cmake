find_package(jsoncpp CONFIG QUIET)

if(NOT TARGET JsonCpp::JsonCpp)
    if(JsonCpp_FOUND)
        message(STATUS "Found jsoncpp via config file: ${JsonCpp_DIR}")
        if(NOT JsonCpp_LIBRARY)
            if(TARGET jsoncpp)
                set(JsonCpp_LIBRARY jsoncpp)
            elseif(TARGET JsonCpp::JsonCpp)
                set(JsonCpp_LIBRARY JsonCpp::JsonCpp)
            endif()
        endif()
    else()
        # Try to locate it via pkg-config. If that yields a result, this is
        # considered authoritative. This MUST be done in order to ensure that it
        # works correctly in multi-arch builds.
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(PC_JSONCPP QUIET IMPORTED_TARGET GLOBAL jsoncpp)
            if(PC_JSONCPP_FOUND)
                add_library(JsonCpp::JsonCpp ALIAS PkgConfig::PC_JSONCPP)
            endif()
        endif()

        if(NOT PC_JSONCPP_FOUND)
            find_path(JsonCpp_INCLUDE_DIR
                    NAMES json/json.h
                    HINTS ${PC_JSONCPP_INCLUDEDIR} ${PC_JSONCPP_INCLUDE_DIRS})
            find_library(JsonCpp_LIBRARY
                        NAMES jsoncpp
                        HINTS ${PC_JSONCPP_LIBDIR} ${PC_JSONCPP_LIBRARY_DIRS})

            include(FindPackageHandleStandardArgs)
            find_package_handle_standard_args(JsonCpp DEFAULT_MSG JsonCpp_LIBRARY JsonCpp_INCLUDE_DIR)

            mark_as_advanced(JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY)
        endif()
    endif()

    if (JsonCpp_FOUND)
        add_library(JsonCpp::JsonCpp INTERFACE IMPORTED)
        set_target_properties(JsonCpp::JsonCpp PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${JsonCpp_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${JsonCpp_LIBRARY}")
        if(NOT JsonCpp_LIBRARY)
            unset(JsonCpp_LIBRARY CACHE)
        endif()
        if(NOT JsonCpp_INCLUDE_DIR)
            unset(JsonCpp_INCLUDE_DIR CACHE)
        endif()
    endif()
endif()
