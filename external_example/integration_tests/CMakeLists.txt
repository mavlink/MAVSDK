add_executable(external_example_integration_tests_runner
    ${PROJECT_SOURCE_DIR}/core/unittests_main.cpp
    hello_world.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/external_example
)

target_link_libraries(external_example_integration_tests_runner
    dronecode_sdk
    dronecode_sdk_example
    gtest
    gtest_main
    gmock
)
