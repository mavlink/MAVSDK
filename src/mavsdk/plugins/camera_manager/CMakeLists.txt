target_sources(mavsdk
    PRIVATE
    # camera_manager.cpp
    camera_manager_impl.cpp
    # camera_definition_files/generated/camera_definition_files.cpp
)

target_include_directories(mavsdk
    # PRIVATE
    # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/generated>
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/camera_manager/camera_manager.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/camera_manager
)

#list(APPEND UNIT_TEST_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/camera_manager_test.cpp
#)
#set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)

