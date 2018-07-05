add_library(dronecode_sdk_logging ${PLUGIN_LIBRARY_TYPE}
    logging.cpp
    logging_impl.cpp
)

target_link_libraries(dronecode_sdk_logging
    dronecode_sdk
)

install(FILES
    logging.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_logging
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
