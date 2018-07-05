add_library(dronecode_sdk_info ${PLUGIN_LIBRARY_TYPE}
    info.cpp
    info_impl.cpp
)

target_link_libraries(dronecode_sdk_info
    dronecode_sdk
)

install(FILES
    info.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_info
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
