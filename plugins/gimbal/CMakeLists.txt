add_library(dronecode_sdk_gimbal ${PLUGIN_LIBRARY_TYPE}
    gimbal.cpp
    gimbal_impl.cpp
)

target_link_libraries(dronecode_sdk_gimbal
    dronecode_sdk
)

set_target_properties(dronecode_sdk_gimbal
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    gimbal.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_gimbal
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
