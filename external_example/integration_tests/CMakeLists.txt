add_executable(external_example_integration_tests_runner
    ${CMAKE_SOURCE_DIR}/core/unittests_main.cpp
    hello_world.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/external_example
)

target_link_libraries(external_example_integration_tests_runner
    dronecore
    dronecore_example
    gtest
    gtest_main
    gmock
)
