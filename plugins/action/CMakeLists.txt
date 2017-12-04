add_library(dronecore_action SHARED
    action.cpp
    action_impl.cpp
)

install(FILES
    action.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_action
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
