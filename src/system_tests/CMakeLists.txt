add_executable(system_tests_runner
    ../mavsdk/core/unittests_main.cpp
    camera_take_photo.cpp
    component_information.cpp
    action_arm_disarm.cpp
    param_set_and_get.cpp
    param_get_all.cpp
    param_custom_set_and_get.cpp
    param_get_all.cpp
    mission_raw_upload.cpp
    telemetry_subscription.cpp
)

target_include_directories(system_tests_runner
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

target_link_libraries(system_tests_runner
    PRIVATE
    mavsdk
    gtest
    gtest_main
    gmock
)
