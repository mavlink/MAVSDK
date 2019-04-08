add_library(dronecode_sdk_mavlink_passthrough ${PLUGIN_LIBRARY_TYPE}
    mavlink_passthrough.cpp
    mavlink_passthrough_impl.cpp
)

target_link_libraries(dronecode_sdk_mavlink_passthrough
    dronecode_sdk
)

set_target_properties(dronecode_sdk_mavlink_passthrough
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/mavlink_passthrough/mavlink_passthrough.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/mavlink_passthrough
)

install(DIRECTORY
    ../../third_party/mavlink/include/mavlink
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/mavlink_passthrough
)

install(TARGETS dronecode_sdk_mavlink_passthrough
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_mavlink_passthrough
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/mavlink/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
