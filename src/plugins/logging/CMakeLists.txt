add_library(mavsdk_logging
    logging.cpp
    logging_impl.cpp
)

target_link_libraries(mavsdk_logging
    mavsdk
)

set_target_properties(mavsdk_logging
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS mavsdk_logging
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/logging/logging.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/logging
)
