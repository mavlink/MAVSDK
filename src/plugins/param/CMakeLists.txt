add_library(dronecode_sdk_param
    param.cpp
    param_impl.cpp
)

target_link_libraries(dronecode_sdk_param
    dronecode_sdk
)

set_target_properties(dronecode_sdk_param
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/param/param.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/param
)

install(TARGETS dronecode_sdk_param
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_param
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
