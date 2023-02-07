target_sources(mavsdk
    PRIVATE
    mavlink_passthrough.cpp
    mavlink_passthrough_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/mavlink_passthrough/mavlink_passthrough.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mavlink_passthrough
)
