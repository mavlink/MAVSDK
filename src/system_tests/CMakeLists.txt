add_executable(system_tests_runner
    camera_take_photo.cpp
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
    system_tests_runner.cpp
)

target_include_directories(system_tests_runner
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk/core
)

hunter_add_package(GTest)
find_package(GTest REQUIRED)

target_link_libraries(system_tests_runner
    PRIVATE
    mavsdk
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
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
