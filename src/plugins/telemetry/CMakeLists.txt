add_library(mavsdk_telemetry
    telemetry.cpp
    telemetry_impl.cpp
    math_conversions.cpp
    path_checker.cpp
)

target_link_libraries(mavsdk_telemetry
    mavsdk
)

set_target_properties(mavsdk_telemetry
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_telemetry PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/path_checker_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)

install(TARGETS mavsdk_telemetry
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/telemetry/telemetry.h
    include/plugins/telemetry/path_checker.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/telemetry
)
