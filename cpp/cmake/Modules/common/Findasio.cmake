# Findasio.cmake
#
# Locates standalone Asio (header-only, BSL-1.0) and provides the
# asio::asio INTERFACE target with ASIO_STANDALONE defined.
#
# Search order:
#   1. asio-config.cmake from the MAVSDK superbuild (DEPS_INSTALL_PATH)
#   2. A system-installed asio.hpp (e.g. from libasio-dev on Debian/Ubuntu)

# 1 — Try the config-file package installed by the superbuild
find_package(asio CONFIG QUIET)

if(TARGET asio::asio)
    return()
endif()

# 2 — Fall back to locating the header directly
find_path(ASIO_INCLUDE_DIR
    NAMES asio.hpp
    PATH_SUFFIXES include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(asio DEFAULT_MSG ASIO_INCLUDE_DIR)

mark_as_advanced(ASIO_INCLUDE_DIR)

if(asio_FOUND AND NOT TARGET asio::asio)
    add_library(asio::asio INTERFACE IMPORTED)
    set_target_properties(asio::asio PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ASIO_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "ASIO_STANDALONE"
    )
    if(WIN32)
        set_property(TARGET asio::asio APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ws2_32 mswsock)
    endif()
endif()
