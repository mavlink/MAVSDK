add_library(dronecore_camera ${PLUGIN_LIBRARY_TYPE}
    camera.cpp
    camera_impl.cpp
)

target_link_libraries(dronecore_camera
    dronecore
)

install(FILES
    camera.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_camera
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
