if(NOT MSVC AND NOT MINGW)
    add_resource(camera_definition_files)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/e90.xml VARIABLE e90xml TEXT)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/e50.xml VARIABLE e50xml TEXT)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/cgoet.xml VARIABLE cgoetxml TEXT)
    link_resource_file(camera_definition_files FILE ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_files/e10t.xml VARIABLE e10txml TEXT)

    get_property(RSC_CPP_FILE TARGET camera_definition_files PROPERTY _AR_SRC_FILE)
    get_property(RSC_H_DIR TARGET camera_definition_files PROPERTY _AR_H_DIR)

    include_directories(${RSC_H_DIR})
endif()

add_library(mavsdk_camera
    camera.cpp
    camera_impl.cpp
    camera_definition.cpp
    ${RSC_CPP_FILE}
)

target_link_libraries(mavsdk_camera
    PUBLIC
    mavsdk
    PRIVATE
    tinyxml2::tinyxml2
    CURL::libcurl
)

set_target_properties(mavsdk_camera
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_camera PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_camera
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/camera/camera.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/camera
)

list(APPEND UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/camera_definition_test.cpp
)
set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
