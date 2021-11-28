add_library(mavsdk_telemetry
    telemetry.cpp
    telemetry_impl.cpp
    math_conversions.cpp
)

target_link_libraries(mavsdk_telemetry
    mavsdk
)

set_target_properties(mavsdk_telemetry
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_telemetry PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_telemetry
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/telemetry/telemetry.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/telemetry
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/math_conversions_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
