target_sources(mavsdk
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/math_conversions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/telemetry/telemetry.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/telemetry
)
