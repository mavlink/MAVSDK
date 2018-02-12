# SYSTEM because we don't want warnings for gtest headers.
include_directories(SYSTEM third_party/gtest/googletest/include)
include_directories(SYSTEM third_party/gtest/googlemock/include)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/third_party/mavlink/include)

# Build unit tests
add_executable(unit_tests_runner
    ${CMAKE_SOURCE_DIR}/core/global_include_test.cpp
    ${CMAKE_SOURCE_DIR}/core/mavlink_channels_test.cpp
    ${CMAKE_SOURCE_DIR}/core/unittests_main.cpp
    # TODO: add this again
    #${CMAKE_SOURCE_DIR}/core/http_loader_test.cpp
    ${CMAKE_SOURCE_DIR}/core/timeout_handler_test.cpp
    ${CMAKE_SOURCE_DIR}/core/call_every_handler_test.cpp
    ${CMAKE_SOURCE_DIR}/core/curl_test.cpp
    ${plugin_unittest_source_files}
)

if (MSVC)
    # We need this to prevent linking errors from happening in the Windows build.
    target_compile_definitions(unit_tests_runner PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
    target_compile_options(unit_tests_runner PUBLIC "/wd4251" "/wd4275")
endif()

target_compile_definitions(unit_tests_runner PRIVATE FAKE_TIME=1)

set_target_properties(unit_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(unit_tests_runner
    dronecore
    dronecore_mission
    gtest
    gtest_main
    gmock
)

add_test(unit_tests
    unit_tests_runner
)
