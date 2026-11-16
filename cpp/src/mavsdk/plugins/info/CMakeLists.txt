target_sources(mavsdk
    PRIVATE
    info.cpp
    info_impl.cpp
    binary_to_str.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/info/info.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/info
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/binary_to_str_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
