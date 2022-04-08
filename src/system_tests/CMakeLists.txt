add_executable(system_tests_runner
    ../mavsdk/core/unittests_main.cpp
    camera_take_photo.cpp
    action_arm_disarm.cpp
    system_tests_helper.cpp
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
