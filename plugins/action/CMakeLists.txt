add_library(dronecode_sdk_action ${PLUGIN_LIBRARY_TYPE}
    action.cpp
    action_impl.cpp
)

target_link_libraries(dronecode_sdk_action
    dronecode_sdk
)

install(FILES
    action.h
    action_result.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_action
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
