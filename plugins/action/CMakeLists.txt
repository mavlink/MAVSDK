add_library(dronecode_sdk_action ${PLUGIN_LIBRARY_TYPE}
    action.cpp
    action_impl.cpp
)

target_link_libraries(dronecode_sdk_action
    dronecode_sdk
)

set_target_properties(dronecode_sdk_action
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(dronecode_sdk_action
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(FILES
    include/plugins/action/action.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_action
    DESTINATION ${dronecode_sdk_install_lib_dir}
)
