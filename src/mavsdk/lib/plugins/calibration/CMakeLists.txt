add_library(mavsdk_calibration
    calibration.cpp
    calibration_impl.cpp
    calibration_statustext_parser.cpp
)

target_link_libraries(mavsdk_calibration
    mavsdk
)

set_target_properties(mavsdk_calibration
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_calibration PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_calibration
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/calibration/calibration.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/calibration
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration_statustext_parser_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
