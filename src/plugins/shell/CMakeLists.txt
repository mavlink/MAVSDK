add_library(mavsdk_shell
        shell.cpp
        shell_impl.cpp
        )

target_link_libraries(mavsdk_shell
        mavsdk
        )

set_target_properties(mavsdk_shell
        PROPERTIES COMPILE_FLAGS ${warnings}
        VERSION ${MAVSDK_VERSION_STRING}
        SOVERSION ${MAVSDK_SOVERSION_STRING}
)

target_include_directories(mavsdk_shell PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/mavsdk>
        )

install(TARGETS mavsdk_shell
        EXPORT mavsdk-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(FILES
        include/plugins/shell/shell.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/shell
        )
