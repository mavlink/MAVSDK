target_sources(mavsdk
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mission_raw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mission_raw_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/mission_raw/mission_raw.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mission_raw
)