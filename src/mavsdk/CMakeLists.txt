
target_include_directories(mavsdk
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/lib>
    )


install(FILES
    include/mavsdk/connection_result.h
    include/mavsdk/deprecated.h
    include/mavsdk/system.h
    include/mavsdk/mavsdk.h
    include/mavsdk/plugin_base.h
    include/mavsdk/geometry.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mavsdk"
)

add_subdirectory(lib)
