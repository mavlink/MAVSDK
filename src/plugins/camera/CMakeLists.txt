if(NOT MSVC)
    add_resource(camera_definition_files)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/e90.xml VARIABLE e90xml TEXT)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/e50.xml VARIABLE e50xml TEXT)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/cgoet.xml VARIABLE cgoetxml TEXT)

    get_property(RSC_CPP_FILE TARGET camera_definition_files PROPERTY _AR_SRC_FILE)
    get_property(RSC_H_DIR TARGET camera_definition_files PROPERTY _AR_H_DIR)

    include_directories(${RSC_H_DIR})
endif()

add_library(dronecode_sdk_camera
    camera.cpp
    camera_impl.cpp
    camera_definition.cpp
    ${RSC_CPP_FILE}
)

target_link_libraries(dronecode_sdk_camera
    dronecode_sdk
    tinyxml2::tinyxml2
    ${CURL_LIBRARY}
)

set_target_properties(dronecode_sdk_camera
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/camera/camera.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/camera
)

install(TARGETS dronecode_sdk_camera
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

