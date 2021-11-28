add_library(mavsdk_mavlink_passthrough
    mavlink_passthrough.cpp
    mavlink_passthrough_impl.cpp
)

target_link_libraries(mavsdk_mavlink_passthrough
    mavsdk
)

set_target_properties(mavsdk_mavlink_passthrough
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_mavlink_passthrough PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_mavlink_passthrough
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/mavlink_passthrough/mavlink_passthrough.h
    ../../core/mavlink_include.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mavlink_passthrough
)

install(DIRECTORY
    ../../third_party/mavlink/include/mavlink
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mavlink_passthrough
)
