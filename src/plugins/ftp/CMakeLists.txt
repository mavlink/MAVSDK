target_sources(mavsdk
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ftp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ftp_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crc32.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/ftp/ftp.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/ftp
)
