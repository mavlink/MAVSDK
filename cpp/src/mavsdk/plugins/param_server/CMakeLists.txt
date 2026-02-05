target_sources(mavsdk
    PRIVATE
    param_server.cpp
    param_server_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/param_server/param_server.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/param_server
)
