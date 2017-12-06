add_library(dronecore_offboard SHARED
    offboard.cpp
    offboard_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/libs/include
)

install(FILES
    offboard.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_offboard
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
