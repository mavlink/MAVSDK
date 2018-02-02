add_library(dronecore_offboard SHARED
    offboard.cpp
    offboard_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

target_link_libraries(dronecore_offboard
    dronecore
)

install(FILES
    offboard.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_offboard
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
