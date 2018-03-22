add_library(dronecore_camera ${PLUGIN_LIBRARY_TYPE}
    camera.cpp
    camera_impl.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
)

target_link_libraries(dronecore_camera
    dronecore
)

install(FILES
    camera.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_camera
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/camera/camera_unit_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
