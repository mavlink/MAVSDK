# WINGTRA: DISABLE target_sources(mavsdk
# WINGTRA: DISABLE     PRIVATE
# WINGTRA: DISABLE     gimbal.cpp
# WINGTRA: DISABLE     gimbal_impl.cpp
# WINGTRA: DISABLE     gimbal_protocol_v1.cpp
# WINGTRA: DISABLE     gimbal_protocol_v2.cpp
# WINGTRA: DISABLE )
# WINGTRA: DISABLE 
# WINGTRA: DISABLE target_include_directories(mavsdk PUBLIC
# WINGTRA: DISABLE     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
# WINGTRA: DISABLE     $<INSTALL_INTERFACE:include>
# WINGTRA: DISABLE     )
# WINGTRA: DISABLE 
# WINGTRA: DISABLE install(FILES
# WINGTRA: DISABLE     include/plugins/gimbal/gimbal.h
# WINGTRA: DISABLE     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mavsdk/plugins/gimbal
# WINGTRA: DISABLE )
