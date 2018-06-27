add_library(dronecore_orbit ${PLUGIN_LIBRARY_TYPE}
    orbit.cpp
    orbit_impl.cpp
)

target_link_libraries(dronecore_orbit
    dronecore
)

install(FILES
    orbit.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_orbit
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
