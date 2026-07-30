target_sources(mavsdk
    PRIVATE
    gimbal.cpp
    gimbal_impl.cpp
    gimbal_device_matching.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/gimbal/gimbal.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/gimbal
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gimbal_device_matching_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
