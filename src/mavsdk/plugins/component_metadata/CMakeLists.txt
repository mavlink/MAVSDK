target_sources(mavsdk
    PRIVATE
        component_metadata.cpp
        component_metadata_impl.cpp
        file_cache.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
        include/plugins/component_metadata/component_metadata.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/component_metadata
)

list(APPEND UNIT_TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/file_cache_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/component_metadata_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
