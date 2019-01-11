add_library(dronecode_sdk_mission ${PLUGIN_LIBRARY_TYPE}
    mission.cpp
    mission_impl.cpp
    mission_item.cpp
    mission_item_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    SYSTEM third_party/json11
)

set_target_properties(dronecode_sdk_mission
    PROPERTIES COMPILE_FLAGS ${warnings}
)

# JSON parser library for parsing QGC plan for Mission
add_subdirectory(third_party/json11)

target_link_libraries(dronecode_sdk_mission
    dronecode_sdk
    json11
)

install(FILES
    include/plugins/mission/mission.h
    include/plugins/mission/mission_item.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/mission
)

install(TARGETS dronecode_sdk_mission
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_mission
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(ANDROID)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/third_party/json11/libjson11.a
        DESTINATION ${lib_path}
    )
endif()

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mission_import_qgc_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
