add_library(dronecode_sdk_offboard ${PLUGIN_LIBRARY_TYPE}
    offboard.cpp
    offboard_impl.cpp
)

target_link_libraries(dronecode_sdk_offboard
    dronecode_sdk
)

set_target_properties(dronecode_sdk_offboard
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/offboard/offboard.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/offboard
)

install(TARGETS dronecode_sdk_offboard
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_offboard
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
