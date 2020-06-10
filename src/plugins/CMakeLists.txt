include_directories(
    ${PROJECT_SOURCE_DIR}/core
    SYSTEM  ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

add_subdirectory(action)
add_subdirectory(calibration)
add_subdirectory(camera)
add_subdirectory(follow_me)
add_subdirectory(geofence)
add_subdirectory(gimbal)
add_subdirectory(info)
add_subdirectory(log_files)
add_subdirectory(ftp)
add_subdirectory(mission)
add_subdirectory(mission_raw)
add_subdirectory(mocap)
add_subdirectory(offboard)
add_subdirectory(param)
add_subdirectory(shell)
add_subdirectory(telemetry)
add_subdirectory(tune)
add_subdirectory(failure)

if (ENABLE_MAVLINK_PASSTHROUGH)
    message(STATUS "Include MAVLink passthrough plugin")
    add_subdirectory(mavlink_passthrough)
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
