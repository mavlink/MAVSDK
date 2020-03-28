add_library(mavsdk_gimbal_manager
    gimbal_manager.cpp
    gimbal_manager_impl.cpp
)

target_link_libraries(mavsdk_gimbal_manager
    mavsdk
)

set_target_properties(mavsdk_gimbal_manager
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_gimbal_manager PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
)

install(TARGETS mavsdk_gimbal_manager
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    include/plugins/gimbal_manager/gimbal_manager.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/gimbal_manager
)
