add_library(mavsdk_mission_raw
    mission_raw.cpp
    mission_raw_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
)

set_target_properties(mavsdk_mission_raw
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_link_libraries(mavsdk_mission_raw
    mavsdk
)

target_include_directories(mavsdk_mission_raw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_mission_raw
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/mission_raw/mission_raw.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mission_raw
)
