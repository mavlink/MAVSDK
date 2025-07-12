# Generate embedded XML header
set(MAVLINK_XML_SOURCE "${CMAKE_BINARY_DIR}/third_party/mavlink/mavlink/src/mavlink/message_definitions/v1.0/common.xml")
set(EMBEDDED_XML_HEADER "${CMAKE_CURRENT_BINARY_DIR}/embedded_mavlink_xml.h")

add_custom_command(
    OUTPUT ${EMBEDDED_XML_HEADER}
    COMMAND ${CMAKE_COMMAND} 
        -DXML_SOURCE_PATH=${MAVLINK_XML_SOURCE}
        -DOUTPUT_HEADER=${EMBEDDED_XML_HEADER}
        -P ${CMAKE_SOURCE_DIR}/cmake/generate_embedded_xml.cmake
    DEPENDS ${MAVLINK_XML_SOURCE}  # Depend on the XML file instead of the target
    COMMENT "Generating embedded MAVLink XML header"
    VERBATIM
)

# Add custom target to ensure header is generated
add_custom_target(generate_mavlink_xml_header DEPENDS ${EMBEDDED_XML_HEADER})

target_sources(mavsdk
    PRIVATE
    mavlink_direct.cpp
    mavlink_direct_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# Ensure the header is generated before building mavlink_direct files
add_dependencies(mavsdk generate_mavlink_xml_header)

# Make sure the source files know about the generated header
set_source_files_properties(mavlink_direct_impl.cpp PROPERTIES OBJECT_DEPENDS ${EMBEDDED_XML_HEADER})

install(FILES
    include/plugins/mavlink_direct/mavlink_direct.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mavlink_direct
)