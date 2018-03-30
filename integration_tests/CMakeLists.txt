# SYSTEM because we don't want warnings for gtest headers.
include_directories(
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/gtest/googletest/include
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/gtest/googlemock/include
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/mavlink/include
)

# These are consumed in ../CMakeLists.txt
set(integration_tests_src ${integration_tests_src} PARENT_SCOPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTESTING")

# `make test` does not show output, but `make check` does
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# This includes all GTests that run integration tests
add_executable(integration_tests_runner
    ../core/unittests_main.cpp
    simple_connect.cpp
    async_connect.cpp
    telemetry_simple.cpp
    telemetry_async.cpp
    telemetry_modes.cpp
    telemetry_health.cpp
    simple_hover.cpp
    async_hover.cpp
    takeoff_and_kill.cpp
    offboard_velocity.cpp
    logging.cpp
    info.cpp
    mission.cpp
    mission_change_speed.cpp
    mission_survey.cpp
    gimbal.cpp
    transition_multicopter_fixedwing.cpp
    follow_me.cpp
    multi_components.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}
)

set_target_properties(integration_tests_runner
    PROPERTIES COMPILE_FLAGS ${warnings}
)

target_link_libraries(integration_tests_runner
    dronecore
    dronecore_telemetry
    dronecore_action
    dronecore_mission
    dronecore_offboard
    dronecore_logging
    dronecore_info
    dronecore_gimbal
    dronecore_follow_me
    gtest
    gtest_main
    gmock
)

if (MSVC)
    # We need this to prevent linking errors from happening in the Windows build.
    target_compile_definitions(integration_tests_runner PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
    target_compile_options(integration_tests_runner PUBLIC "/wd4251" "/wd4275")
endif()

add_test(integration_tests
    integration_tests_runner
)

add_custom_command(TARGET integration_tests_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/start_px4_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_custom_command(TARGET integration_tests_runner
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/stop_px4_sitl.sh
        ${CMAKE_CURRENT_BINARY_DIR}
)

