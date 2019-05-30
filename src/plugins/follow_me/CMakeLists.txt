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

target_include_directories(dronecode_sdk_follow_me PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/dronecode_sdk>
    )

install(TARGETS dronecode_sdk_follow_me
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/follow_me/follow_me.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dronecode_sdk/plugins/follow_me
)
