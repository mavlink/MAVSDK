add_library(mavsdk_path_checker
    path_checker.cpp
)

target_link_libraries(mavsdk_path_checker
    PRIVATE
    mavsdk_telemetry
)

set_target_properties(mavsdk_path_checker
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_path_checker
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS mavsdk_path_checker
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    path_checker.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/path_checker/"
)