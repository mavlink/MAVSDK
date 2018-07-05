add_library(dronecore_calibration ${PLUGIN_LIBRARY_TYPE}
    calibration.cpp
    calibration_impl.cpp
    calibration_statustext_parser.cpp
)

target_link_libraries(dronecore_calibration
    dronecore
)

install(FILES
    calibration.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_calibration
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration_statustext_parser_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
