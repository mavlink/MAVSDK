add_library(dronecode_sdk_telemetry ${PLUGIN_LIBRARY_TYPE}
    telemetry.cpp
    telemetry_impl.cpp
    math_conversions.cpp
)

target_link_libraries(dronecode_sdk_telemetry
    dronecode_sdk
)

install(FILES
    telemetry.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_telemetry
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
