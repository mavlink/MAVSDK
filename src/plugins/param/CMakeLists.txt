add_library(dronecode_sdk_param
    param.cpp
    param_impl.cpp
)

target_link_libraries(dronecode_sdk_param
    dronecode_sdk
)

set_target_properties(dronecode_sdk_param
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_param
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
