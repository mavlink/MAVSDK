add_library(mavsdk_action
    action.cpp
    action_impl.cpp
)

target_link_libraries(mavsdk_action
    mavsdk
)

set_target_properties(mavsdk_action
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_action PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_action
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/action/action.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/action
)
