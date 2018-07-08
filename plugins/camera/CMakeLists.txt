add_library(dronecode_sdk_camera ${PLUGIN_LIBRARY_TYPE}
    camera.cpp
    camera_impl.cpp
    camera_definition.cpp
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
    camera.h
    DESTINATION ${dronecode_sdk_install_include_dir}
)

install(TARGETS dronecode_sdk_camera
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)

