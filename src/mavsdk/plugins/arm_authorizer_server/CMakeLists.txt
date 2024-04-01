target_sources(mavsdk
    PRIVATE
    arm_authorizer_server.cpp
    arm_authorizer_server_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/arm_authorizer_server/arm_authorizer_server.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/arm_authorizer_server
)