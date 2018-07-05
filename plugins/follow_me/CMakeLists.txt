add_library(dronecode_sdk_follow_me ${PLUGIN_LIBRARY_TYPE}
    follow_me.cpp
    follow_me_impl.cpp
)

target_link_libraries(dronecode_sdk_follow_me
    dronecode_sdk
)

install(FILES
    follow_me.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_follow_me
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
