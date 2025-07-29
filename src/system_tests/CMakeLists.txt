add_executable(system_tests_runner
    camera_mode.cpp
    camera_take_photo.cpp
    camera_take_photo_interval.cpp
    camera_format_storage.cpp
    camera_settings.cpp
    camera_storage.cpp
    camera_list_cameras.cpp
    camera_video_stream.cpp
    component_metadata.cpp
    action_arm_disarm.cpp
    param_set_and_get.cpp
    param_get_all.cpp
    param_custom_set_and_get.cpp
    param_get_all.cpp
    mission_raw_upload.cpp
    telemetry_subscription.cpp
    fs_helpers.cpp
    ftp_download_file.cpp
    ftp_download_file_burst.cpp
    ftp_remove_file.cpp
    ftp_upload_file.cpp
    ftp_rename_file.cpp
    ftp_create_dir.cpp
    ftp_remove_dir.cpp
    ftp_compare_files.cpp
    ftp_list_dir.cpp
    #mavlink_direct.cpp
    system_tests_runner.cpp
)

target_include_directories(system_tests_runner
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

find_package(GTest REQUIRED)

find_package(jsoncpp REQUIRED)

target_link_libraries(system_tests_runner
    PRIVATE
    mavsdk
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    JsonCpp::JsonCpp
)

set_target_properties(system_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

if(ENABLE_CPPTRACE)
    find_package(cpptrace REQUIRED)
    target_link_libraries(system_tests_runner
        PRIVATE
        cpptrace::cpptrace
    )
    target_compile_definitions(system_tests_runner
        PRIVATE
        -DENABLE_CPPTRACE
    )
endif()

# Standalone tests without GTest
add_executable(standalone_param_test
    standalone_param_test.cpp
)

target_include_directories(standalone_param_test
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

target_link_libraries(standalone_param_test
    PRIVATE
    mavsdk
)

set_target_properties(standalone_param_test
    PROPERTIES COMPILE_FLAGS ${warnings}
)

add_executable(standalone_ftp_test
    standalone_ftp_test.cpp
    fs_helpers.cpp
)

target_include_directories(standalone_ftp_test
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

target_link_libraries(standalone_ftp_test
    PRIVATE
    mavsdk
)

set_target_properties(standalone_ftp_test
    PROPERTIES COMPILE_FLAGS ${warnings}
)
