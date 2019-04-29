add_library(dronecode_sdk_telemetry
    telemetry.cpp
    telemetry_impl.cpp
    math_conversions.cpp
)

target_link_libraries(dronecode_sdk_telemetry
    dronecode_sdk
)

set_target_properties(dronecode_sdk_telemetry
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(TARGETS dronecode_sdk_telemetry
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
