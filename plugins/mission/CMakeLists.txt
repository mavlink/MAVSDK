add_library(dronecore_mission ${PLUGIN_LIBRARY_TYPE}
    mission.cpp
    mission_impl.cpp
    mission_item.cpp
    mission_item_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
    SYSTEM libs/json11
)

# JSON parser library for parsing QGC plan for Mission
add_subdirectory(libs/json11)

if(ANDROID)
    target_link_libraries(dronecore_mission
        log
    )
endif()

target_link_libraries(dronecore_mission
    dronecore
    json11
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

set(unittest_source_files
    mission_import_qgc_test.cpp
    PARENT_SCOPE
)

set(impl_header_files
    mission_impl.h
    PARENT_SCOPE
)
