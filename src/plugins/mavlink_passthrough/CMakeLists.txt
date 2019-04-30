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

target_include_directories(dronecode_sdk_mavlink_passthrough PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/dronecode_sdk>
    )

install(TARGETS dronecode_sdk_mavlink_passthrough
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    mavlink_passthrough.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dronecode_sdk/plugins/mavlink_passthrough
)

install(DIRECTORY
    ../../third_party/mavlink/include/mavlink
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dronecode_sdk/plugins/mavlink_passthrough
)
