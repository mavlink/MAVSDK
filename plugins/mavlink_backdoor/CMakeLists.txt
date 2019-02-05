add_library(dronecode_sdk_mavlink_backdoor ${PLUGIN_LIBRARY_TYPE}
    mavlink_backdoor.cpp
    mavlink_backdoor_impl.cpp
)

target_link_libraries(dronecode_sdk_mavlink_backdoor
    dronecode_sdk
)

set_target_properties(dronecode_sdk_mavlink_backdoor
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/mavlink_backdoor/mavlink_backdoor.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/mavlink_backdoor
)

install(DIRECTORY
    ../../third_party/mavlink/include
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_mavlink_backdoor
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_mavlink_backdoor
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/mavlink/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
