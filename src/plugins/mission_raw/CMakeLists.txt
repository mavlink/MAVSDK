add_library(dronecode_sdk_mission_raw
    mission_raw.cpp
    mission_raw_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
)

set_target_properties(dronecode_sdk_mission_raw
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(dronecode_sdk_mission_raw
    dronecode_sdk
)

target_include_directories(dronecode_sdk_mission_raw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/dronecode_sdk>
    )

install(TARGETS dronecode_sdk_mission_raw
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    mission_raw.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dronecode_sdk/plugins/mission_raw
)
