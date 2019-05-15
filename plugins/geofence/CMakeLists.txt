add_library(dronecode_sdk_geofence ${PLUGIN_LIBRARY_TYPE}
    geofence.cpp
    geofence_impl.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
)

set_target_properties(dronecode_sdk_geofence
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(dronecode_sdk_geofence
    dronecode_sdk
)

install(FILES
    include/plugins/geofence/geofence.h
    DESTINATION ${dronecode_sdk_install_include_dir}/plugins/geofence
)

install(TARGETS dronecode_sdk_geofence
    #EXPORT dronecode_sdk-targets
    DESTINATION ${dronecode_sdk_install_lib_dir}
)

target_include_directories(dronecode_sdk_geofence
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
