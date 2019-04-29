add_library(dronecode_sdk_gimbal
    gimbal.cpp
    gimbal_impl.cpp
)

target_link_libraries(dronecode_sdk_gimbal
    dronecode_sdk
)

set_target_properties(dronecode_sdk_gimbal
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_gimbal
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
