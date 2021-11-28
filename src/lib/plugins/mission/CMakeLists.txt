target_sources(mavsdk
    PRIVATE
    mission.cpp
    mission_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/mission/mission.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mission
)
