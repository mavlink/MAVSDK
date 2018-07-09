# SYSTEM because we don't want warnings for gtest headers.
include_directories(
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/gtest/googletest/include
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/gtest/googlemock/include
    SYSTEM ${PROJECT_SOURCE_DIR}/third_party/mavlink/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTESTING")

# `make test` does not show output, but `make check` does
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# This includes all GTests that run integration tests
add_executable(integration_tests_runner
    ../core/unittests_main.cpp
    simple_connect.cpp
    async_connect.cpp
    telemetry_simple.cpp
    telemetry_async.cpp
    telemetry_modes.cpp
    telemetry_health.cpp
    simple_hover.cpp
    async_hover.cpp
    takeoff_and_kill.cpp
    offboard_velocity.cpp
    offboard_position.cpp
    logging.cpp
    info.cpp
    mission.cpp
    mission_change_speed.cpp
    mission_survey.cpp
    gimbal.cpp
    transition_multicopter_fixedwing.cpp
    follow_me.cpp
    multi_components.cpp
    calibration.cpp
    camera_take_photo.cpp
    camera_take_photo_interval.cpp
    camera_mode.cpp
    camera_status.cpp
    camera_test_helpers.cpp
    camera_settings.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}
)

set_target_properties(integration_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(integration_tests_runner
    dronecode_sdk
    dronecode_sdk_telemetry
    dronecode_sdk_action
    dronecode_sdk_mission
    dronecode_sdk_offboard
    dronecode_sdk_logging
    dronecode_sdk_info
    dronecode_sdk_gimbal
    dronecode_sdk_follow_me
    dronecode_sdk_camera
    dronecode_sdk_calibration
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
        ${PROJECT_SOURCE_DIR}/start_px4_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(TARGET integration_tests_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_SOURCE_DIR}/stop_px4_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)

