target_sources(mavsdk
    PRIVATE
    modes.cpp
    modes_fallback.cpp
    modes_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/modes/modes.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/modes
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modes_fallback_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
