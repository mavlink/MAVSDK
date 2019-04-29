add_library(dronecode_sdk_mavlink_passthrough
    mavlink_passthrough.cpp
    mavlink_passthrough_impl.cpp
)

target_link_libraries(dronecode_sdk_mavlink_passthrough
    dronecode_sdk
)

set_target_properties(dronecode_sdk_mavlink_passthrough
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_mavlink_passthrough
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
