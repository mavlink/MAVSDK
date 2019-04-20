add_library(dronecode_sdk_follow_me
    follow_me.cpp
    follow_me_impl.cpp
)

target_link_libraries(dronecode_sdk_follow_me
    dronecode_sdk
)

set_target_properties(dronecode_sdk_follow_me
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/follow_me/follow_me.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/follow_me
)

install(TARGETS dronecode_sdk_follow_me
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_follow_me
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

