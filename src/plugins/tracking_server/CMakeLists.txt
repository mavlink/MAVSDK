target_sources(mavsdk
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tracking_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tracking_server_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/tracking_server/tracking_server.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/tracking_server
)
