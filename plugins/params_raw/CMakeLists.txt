add_library(dronecode_sdk_params_raw ${PLUGIN_LIBRARY_TYPE}
    params_raw.cpp
    params_raw_impl.cpp
)

target_link_libraries(dronecode_sdk_params_raw
    dronecode_sdk
)

set_target_properties(dronecode_sdk_params_raw
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/params_raw/params_raw.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_params_raw
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_params_raw
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
