add_library(dronecode_sdk_info
    info.cpp
    info_impl.cpp
)

target_link_libraries(dronecode_sdk_info
    dronecode_sdk
)

set_target_properties(dronecode_sdk_info
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_info
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
