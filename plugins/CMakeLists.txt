include_directories(
    ${PROJECT_SOURCE_DIR}/core
    SYSTEM  ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

if(IOS OR ANDROID)
    set(PLUGIN_LIBRARY_TYPE STATIC)
else()
    set(PLUGIN_LIBRARY_TYPE SHARED)
endif()

add_subdirectory(action)
add_subdirectory(calibration)
add_subdirectory(camera)
add_subdirectory(follow_me)
add_subdirectory(gimbal)
add_subdirectory(info)
add_subdirectory(log_files)
#add_subdirectory(logging) # Not implemented completely
add_subdirectory(mission)
add_subdirectory(mission_raw)
add_subdirectory(offboard)
add_subdirectory(params_raw)
add_subdirectory(telemetry)

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
