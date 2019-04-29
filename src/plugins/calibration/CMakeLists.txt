add_library(dronecode_sdk_calibration
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

install(TARGETS dronecode_sdk_calibration
    EXPORT dronecode-sdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration_statustext_parser_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
