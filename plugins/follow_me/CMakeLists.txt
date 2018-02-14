add_library(dronecore_follow_me ${PLUGIN_LIBRARY_TYPE}
    follow_me.cpp
    follow_me_impl.cpp
)

target_link_libraries(dronecore_follow_me
    dronecore
)

install(FILES
    follow_me.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_follow_me
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
