find_package(JsonCpp REQUIRED)

add_library(mavsdk_mission
    mission.cpp
    mission_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
)

set_target_properties(mavsdk_mission
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(mavsdk_mission
    PUBLIC
    mavsdk
    PRIVATE
    JsonCpp::jsoncpp
)

target_include_directories(mavsdk_mission PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_mission
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/mission/mission.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mission
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mission_import_qgc_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mission_equality_operator_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
