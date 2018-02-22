cmake_minimum_required(VERSION 3.1)

add_executable(unit_tests_backend
    connection_initiator_test.cpp
)

set_target_properties(unit_tests_backend PROPERTIES COMPILE_FLAGS ${warnings})

target_include_directories(unit_tests_backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(unit_tests_backend
    backend
    gtest
    gmock
    gmock_main
)

add_test(unit_tests unit_tests_backend)
