cmake_minimum_required(VERSION 3.1)

add_executable(external_example_integration_tests_runner
    ${PROJECT_SOURCE_DIR}/core/unittests_main.cpp
    hello_world.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/integration_tests
    ${PROJECT_SOURCE_DIR}/third_party/gtest/googletest/include
    ${PROJECT_SOURCE_DIR}/third_party/gtest/googlemock/include
)

target_link_libraries(external_example_integration_tests_runner
    mavsdk
    mavsdk_example
    gtest
    gtest_main
    gmock
)
