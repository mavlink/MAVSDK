find_package(jsoncpp REQUIRED)

add_library(mavsdk_custom_action
    custom_action.cpp
    custom_action_impl.cpp
)

target_link_libraries(mavsdk_custom_action
    PUBLIC
    mavsdk
    PRIVATE
    JsonCpp::JsonCpp
)

set_target_properties(mavsdk_custom_action
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_custom_action PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_custom_action
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/custom_action/custom_action.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/custom_action
)
