add_library(dronecode_sdk_telemetry ${PLUGIN_LIBRARY_TYPE}
    telemetry.cpp
    telemetry_impl.cpp
    math_conversions.cpp
)

target_link_libraries(dronecode_sdk_telemetry
    dronecode_sdk
)

set_target_properties(dronecode_sdk_telemetry
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/telemetry/telemetry.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/telemetry
)

install(TARGETS dronecode_sdk_telemetry
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_telemetry
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
