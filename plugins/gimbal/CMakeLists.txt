add_library(dronecore_gimbal ${PLUGIN_LIBRARY_TYPE}
    gimbal.cpp
    gimbal_impl.cpp
)

target_link_libraries(dronecore_gimbal
    dronecore
)

install(FILES
    gimbal.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_gimbal
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
