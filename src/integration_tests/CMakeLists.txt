# SYSTEM because we don't want warnings for gtest headers.
include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/gtest/googletest/include
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/gtest/googlemock/include
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTESTING")

if(MSVC)
    add_compile_options(
         /wd4996 # ignore deprecated
    )
else()
    add_compile_options(
        -Wno-error=deprecated-declarations
    )
endif()

# `make test` does not show output, but `make check` does
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# This includes all GTests that run integration tests
add_executable(integration_tests_runner
    ../core/unittests_main.cpp
    action_hover_async.cpp
    action_hover_sync.cpp
    action_takeoff_and_kill.cpp
    action_transition_multicopter_fixedwing.cpp
    action_goto.cpp
    action_hold.cpp
    calibration.cpp
    camera_mode.cpp
    camera_settings.cpp
    camera_status.cpp
    camera_take_photo.cpp
    camera_take_photo_interval.cpp
    camera_format.cpp
    camera_test_helpers.cpp
    camera_test_helpers.h
    CMakeLists.txt
    follow_me.cpp
    geofence_inclusion.cpp
    gimbal.cpp
    info.cpp
    offboard_attitude.cpp
    #logging.cpp # Not fully implemented
    log_files.cpp
    ftp.cpp
    mission_cancellation.cpp
    mission_change_speed.cpp
    mission.cpp
    mission_rtl.cpp
    mission_raw_mission_changed.cpp
    mission_raw_import_and_fly.cpp
    mission_set_current.cpp
    offboard_acceleration.cpp
    offboard_velocity.cpp
    offboard_position.cpp
    param.cpp
    path_checker.cpp
    path_checker.h
    system_connection_async.cpp
    system_multi_components.cpp
    statustext.cpp
    telemetry_async.cpp
    telemetry_health.cpp
    telemetry_modes.cpp
    telemetry_sync.cpp
    mavlink_passthrough.cpp
    mission_transfer_lossy.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}
)

set_target_properties(integration_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(integration_tests_runner
    mavsdk
    mavsdk_telemetry
    mavsdk_action
    mavsdk_mission
    mavsdk_mission_raw
    mavsdk_offboard
    mavsdk_log_files
    mavsdk_info
    mavsdk_gimbal
    mavsdk_follow_me
    mavsdk_camera
    mavsdk_calibration
    mavsdk_geofence
    mavsdk_param
    mavsdk_ftp
    mavsdk_mavlink_passthrough
    mavsdk_server_utility
    gtest
    gtest_main
    gmock
)

add_test(integration_tests
    integration_tests_runner
)

add_custom_command(TARGET integration_tests_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/start_px4_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(TARGET integration_tests_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/stop_px4_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)
