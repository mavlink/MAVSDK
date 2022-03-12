# WINGTRA DSIABLED target_sources(mavsdk
# WINGTRA DSIABLED     PRIVATE
# WINGTRA DSIABLED     tracking_server.cpp
# WINGTRA DSIABLED     tracking_server_impl.cpp
# WINGTRA DSIABLED )
# WINGTRA DSIABLED 
# WINGTRA DSIABLED target_include_directories(mavsdk PUBLIC
# WINGTRA DSIABLED     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
# WINGTRA DSIABLED     $<INSTALL_INTERFACE:include>
# WINGTRA DSIABLED     )
# WINGTRA DSIABLED 
# WINGTRA DSIABLED install(FILES
# WINGTRA DSIABLED     include/plugins/tracking_server/tracking_server.h
# WINGTRA DSIABLED     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/tracking_server
# WINGTRA DSIABLED )
