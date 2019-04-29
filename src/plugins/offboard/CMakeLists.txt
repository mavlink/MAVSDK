add_library(dronecode_sdk_offboard
    offboard.cpp
    offboard_impl.cpp
)

target_link_libraries(dronecode_sdk_offboard
    dronecode_sdk
)

set_target_properties(dronecode_sdk_offboard
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_offboard
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
