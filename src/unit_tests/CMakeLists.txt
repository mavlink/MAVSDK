find_package(jsoncpp REQUIRED)

add_executable(unit_tests_runner
    ${UNIT_TEST_SOURCES}
)

target_compile_definitions(unit_tests_runner PRIVATE FAKE_TIME=1 TESTING=1)

set_target_properties(unit_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

find_package(GTest REQUIRED)
find_package(tinyxml2 REQUIRED)

target_link_libraries(unit_tests_runner
    mavsdk
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(unit_tests_runner
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../mavsdk/core
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../mavsdk/core
)


add_test(unit_tests unit_tests_runner)
