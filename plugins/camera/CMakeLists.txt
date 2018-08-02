if (UNIX OR APPLE)
    ADD_RESOURCES(camera_definition_e90 camera_definition_files/e90.xml)
    ADD_RESOURCES(camera_definition_e50 camera_definition_files/e50.xml)
    ADD_RESOURCES(camera_definition_cgoet camera_definition_files/cgoet.xml)
endif()

add_library(dronecode_sdk_camera ${PLUGIN_LIBRARY_TYPE}
    camera.cpp
    camera_impl.cpp
    camera_definition.cpp
    ${camera_definition_e90}
    ${camera_definition_e50}
    ${camera_definition_cgoet}
)

target_link_libraries(dronecode_sdk_camera
    dronecode_sdk
    ${TINYXML2_LIBRARY}
    ${CURL_LIBRARY}
)

set_target_properties(dronecode_sdk_camera
    PROPERTIES COMPILE_FLAGS ${warnings}
)

include_directories(
    ${CURL_INCLUDE_DIRS}
)

install(FILES
    include/plugins/camera/camera.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_camera
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_camera
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)

