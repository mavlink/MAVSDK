# `make test` does not show output, but `make check` does
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# This includes all GTests that run integration tests
add_executable(integration_tests_runner
    ../mavsdk/core/unittests_main.cpp
    action_hover_async.cpp
    action_hover_sync.cpp
    action_takeoff_and_kill.cpp
    action_transition_multicopter_fixedwing.cpp
    action_goto.cpp
    action_hold.cpp
    calibration.cpp
    camera_capture_info.cpp
    camera_mode.cpp
    camera_settings.cpp
    camera_status.cpp
    camera_take_photo.cpp
    camera_take_photo_interval.cpp
    camera_format.cpp
    camera_test_helpers.cpp
    connection.cpp
    follow_me.cpp
    geofence.cpp
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
    mission_transition.cpp
    mission_takeoff_land.cpp
    offboard_acceleration.cpp
    offboard_velocity.cpp
    offboard_position.cpp
    param.cpp
    path_checker.cpp
    system_connection_async.cpp
    system_multi_components.cpp
    statustext.cpp
    telemetry_async.cpp
    telemetry_gps_origin.cpp
    telemetry_health.cpp
    telemetry_modes.cpp
    telemetry_sync.cpp
    mavlink_passthrough.cpp
    mission_transfer_lossy.cpp
)

target_include_directories(integration_tests_runner
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

# SYSTEM because we don't want warnings for gtest headers.
target_include_directories(integration_tests_runner
    SYSTEM
    PRIVATE ${PROJECT_SOURCE_DIR}/third_party/gtest/googletest/include
    PRIVATE ${PROJECT_SOURCE_DIR}/third_party/gtest/googlemock/include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../third_party/include
)

if(MSVC)
    target_compile_options(integration_tests_runner
        PRIVATE
         /wd4996 # ignore deprecated
    )
else()
    target_compile_options(integration_tests_runner
        PRIVATE
        -Wno-error=deprecated-declarations
    )
endif()

target_compile_definitions(integration_tests_runner
    PRIVATE
    "-DTESTING")

set_target_properties(integration_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(integration_tests_runner
    PRIVATE
    mavsdk
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
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/start_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(TARGET integration_tests_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/stop_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)
