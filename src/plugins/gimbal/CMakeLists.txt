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

target_include_directories(dronecode_sdk_gimbal PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/dronecode_sdk>
    )

install(TARGETS dronecode_sdk_gimbal
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/gimbal/gimbal.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dronecode_sdk/plugins/gimbal
)
