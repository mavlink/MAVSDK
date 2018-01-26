add_library(dronecore_mission SHARED
    mission.cpp
    mission_impl.cpp
    mission_item.cpp
    mission_item_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

target_link_libraries(dronecore_mission
    dronecore
)

install(FILES
    mission.h
    mission_item.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_mission
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
