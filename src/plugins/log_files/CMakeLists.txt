add_library(dronecode_sdk_log_files ${PLUGIN_LIBRARY_TYPE}
    log_files.cpp
    log_files_impl.cpp
)

target_link_libraries(dronecode_sdk_log_files
    dronecode_sdk
)

set_target_properties(dronecode_sdk_log_files
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/log_files/log_files.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/log_files
)

install(TARGETS dronecode_sdk_log_files
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_log_files
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
