cmake_minimum_required(VERSION 3.1)

add_executable(external_example_integration_tests_runner
    ${CMAKE_SOURCE_DIR}/core/unittests_main.cpp
    hello_world.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/integration_tests
    ${CMAKE_SOURCE_DIR}/third_party/gtest/googletest/include
    ${CMAKE_SOURCE_DIR}/third_party/gtest/googlemock/include
)

target_link_libraries(external_example_integration_tests_runner
    dronecode_sdk
    dronecode_sdk_example
    gtest
    gtest_main
    gmock
)
