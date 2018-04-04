cmake_minimum_required(VERSION 3.1)

add_executable(unit_tests_backend
    action_service_impl_test.cpp
    connection_initiator_test.cpp
    mission_service_impl_test.cpp
    telemetry_service_impl_test.cpp
)

set_target_properties(unit_tests_backend PROPERTIES COMPILE_FLAGS ${warnings})

target_include_directories(unit_tests_backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plugins
    ${CMAKE_SOURCE_DIR}/plugins
    ${CMAKE_BINARY_DIR}/backend/src
)

target_link_libraries(unit_tests_backend
    backend
    gtest
    gmock
    gmock_main
)

add_test(unit_tests unit_tests_backend)
