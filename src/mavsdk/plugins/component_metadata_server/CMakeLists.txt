target_sources(mavsdk
    PRIVATE
        component_metadata_server.cpp
        component_metadata_server_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
        include/plugins/component_metadata_server/component_metadata_server.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/component_information_server
)
