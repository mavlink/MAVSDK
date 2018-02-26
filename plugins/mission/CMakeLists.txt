add_library(dronecore_mission ${PLUGIN_LIBRARY_TYPE}
    mission.cpp
    mission_impl.cpp
    mission_item.cpp
    mission_item_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/include
    SYSTEM third_party/json11
)

# JSON parser library for parsing QGC plan for Mission
add_subdirectory(third_party/json11)

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

if(ANDROID)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/json11/libjson11.so
        DESTINATION ${lib_path}
    )
endif()

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/mission/mission_import_qgc_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
