include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/third_party/mavlink/include)
include_directories(${CURL_INCLUDE_DIRS})

add_executable(unit_tests_runner
    ${UNIT_TEST_SOURCES}
)

target_compile_definitions(unit_tests_runner PRIVATE FAKE_TIME=1)

set_target_properties(unit_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(unit_tests_runner
    dronecore
    dronecore_mission
    dronecore_camera
    gtest
    gtest_main
    gmock
)

add_test(unit_tests unit_tests_runner)
