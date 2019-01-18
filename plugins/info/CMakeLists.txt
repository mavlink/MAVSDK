add_library(dronecode_sdk_info ${PLUGIN_LIBRARY_TYPE}
    info.cpp
    info_impl.cpp
)

target_link_libraries(dronecode_sdk_info
    dronecode_sdk
)

set_target_properties(dronecode_sdk_info
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/info/info.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/info
)

install(TARGETS dronecode_sdk_info
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_info
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
