add_library(dronecode_sdk_calibration ${PLUGIN_LIBRARY_TYPE}
    calibration.cpp
    calibration_impl.cpp
    calibration_statustext_parser.cpp
)

target_link_libraries(dronecode_sdk_calibration
    dronecode_sdk
)

set_target_properties(dronecode_sdk_calibration
    PROPERTIES COMPILE_FLAGS ${warnings}
)

install(FILES
    include/plugins/calibration/calibration.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/calibration
)

install(TARGETS dronecode_sdk_calibration
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_calibration
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration_statustext_parser_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
