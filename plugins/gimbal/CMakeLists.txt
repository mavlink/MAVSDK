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
    include/plugins/gimbal/gimbal.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/gimbal
)

install(TARGETS dronecode_sdk_gimbal
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_gimbal
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
