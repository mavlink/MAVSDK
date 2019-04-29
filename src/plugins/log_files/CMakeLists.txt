add_library(dronecode_sdk_log_files
    log_files.cpp
    log_files_impl.cpp
)

target_link_libraries(dronecode_sdk_log_files
    dronecode_sdk
)

set_target_properties(dronecode_sdk_log_files
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_log_files
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
