find_package(ghcFilesystem REQUIRED)

add_library(mavsdk_mavlink_ftp
    mavlink_ftp.cpp
    mavlink_ftp_impl.cpp
    crc32.cpp
)

target_compile_options(mavsdk_mavlink_ftp
  PRIVATE -fexceptions
)

target_link_libraries(mavsdk_mavlink_ftp
    mavsdk
    ghcFilesystem::ghc_filesystem
)

set_target_properties(mavsdk_mavlink_ftp
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_include_directories(mavsdk_mavlink_ftp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(TARGETS mavsdk_mavlink_ftp
    EXPORT mavsdk-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(FILES
    include/plugins/mavlink_ftp/mavlink_ftp.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mavlink_ftp
)

install(DIRECTORY
    ../../third_party/mavlink/include/mavlink
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/mavlink_ftp
)
