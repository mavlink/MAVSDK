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

install(TARGETS dronecode_sdk_follow_me
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
