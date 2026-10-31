target_sources(mavsdk
    PRIVATE
    modes_server.cpp
    modes_server_impl.cpp
)

target_include_directories(mavsdk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/mavsdk>
    )

install(FILES
    include/plugins/modes_server/modes_server.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/modes_server
)