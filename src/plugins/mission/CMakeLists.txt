find_package(json11 REQUIRED)

add_library(dronecode_sdk_mission
    mission.cpp
    mission_impl.cpp
    mission_item.cpp
    mission_item_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
)

set_target_properties(dronecode_sdk_mission
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(dronecode_sdk_mission
    dronecode_sdk
    json11::json11
)

install(TARGETS dronecode_sdk_mission
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mission_import_qgc_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
