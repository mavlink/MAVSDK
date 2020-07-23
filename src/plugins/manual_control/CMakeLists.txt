add_library(mavsdk_manual_control
    manual_control.cpp
    manual_control_impl.cpp
)

target_link_libraries(mavsdk_manual_control
    mavsdk
)

set_target_properties(mavsdk_manual_control
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_manual_control PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_manual_control
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/manual_control/manual_control.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/manual_control
)