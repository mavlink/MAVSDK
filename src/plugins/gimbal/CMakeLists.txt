add_library(mavsdk_gimbal
    gimbal.cpp
    gimbal_impl.cpp
    gimbal_manager.cpp
    gimbal_manager_impl.cpp
    gimbal_device.cpp
    gimbal_device_impl.cpp
)

target_link_libraries(mavsdk_gimbal
    mavsdk
)

set_target_properties(mavsdk_gimbal
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_gimbal PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_gimbal
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/gimbal/gimbal.h
    include/plugins/gimbal/gimbal_manager.h
    include/plugins/gimbal/gimbal_device.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/gimbal
)
