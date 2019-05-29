add_library(dronecode_sdk_logging
    logging.cpp
    logging_impl.cpp
)

target_link_libraries(dronecode_sdk_logging
    dronecode_sdk
)

set_target_properties(dronecode_sdk_logging
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_logging
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/logging/logging.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/logging
)
