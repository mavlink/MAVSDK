target_sources(mavsdk
    PRIVATE
    gimbal.cpp
    gimbal_impl.cpp
    gimbal_protocol_v1.cpp
    gimbal_protocol_v2.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/gimbal/gimbal.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/gimbal
)
