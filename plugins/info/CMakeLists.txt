add_library(dronecore_info ${PLUGIN_LIBRARY_TYPE}
    info.cpp
    info_impl.cpp
)

target_link_libraries(dronecore_info
    dronecore
)

install(FILES
    info.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_info
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
