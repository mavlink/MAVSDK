target_sources(mavsdk
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/manual_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manual_control_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/manual_control/manual_control.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/manual_control
)
