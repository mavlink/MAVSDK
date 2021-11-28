add_library(mavsdk_transponder
    transponder.cpp
    transponder_impl.cpp
)

target_link_libraries(mavsdk_transponder
    mavsdk
)

set_target_properties(mavsdk_transponder
    PROPERTIES COMPILE_FLAGS ${warnings}
    VERSION ${MAVSDK_VERSION_STRING}
    SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_transponder PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_transponder
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/transponder/transponder.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/transponder
)