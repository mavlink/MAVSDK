include_directories(${PROJECT_SOURCE_DIR}/core)
include_directories(${PROJECT_SOURCE_DIR}/third_party/mavlink/include)

find_package(JsonCpp REQUIRED)

add_executable(unit_tests_runner
    ${UNIT_TEST_SOURCES}
)

target_compile_definitions(unit_tests_runner PRIVATE FAKE_TIME=1)

set_target_properties(unit_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(unit_tests_runner
    mavsdk
    mavsdk_action
    mavsdk_calibration
    mavsdk_camera
    mavsdk_failure
    mavsdk_follow_me
    mavsdk_ftp
    mavsdk_geofence
    mavsdk_gimbal
    mavsdk_info
    mavsdk_log_files
    mavsdk_manual_control
    mavsdk_mavlink_passthrough
    mavsdk_mission
    mavsdk_mission_raw
    mavsdk_mocap
    mavsdk_offboard
    mavsdk_param
    mavsdk_shell
    mavsdk_telemetry
    mavsdk_tune
    CURL::libcurl
    JsonCpp::jsoncpp
    gtest
    gtest_main
    gmock
)

add_test(unit_tests unit_tests_runner)
