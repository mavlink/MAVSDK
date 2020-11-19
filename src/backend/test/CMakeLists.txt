cmake_minimum_required(VERSION 3.1)

add_executable(unit_tests_backend
    action_service_impl_test.cpp
    backend_main.cpp
    camera_service_impl_test.cpp
    connection_initiator_test.cpp
    core_service_impl_test.cpp
    mission_service_impl_test.cpp
    offboard_service_impl_test.cpp
    telemetry_service_impl_test.cpp
    info_service_impl_test.cpp
)

set_target_properties(unit_tests_backend PROPERTIES COMPILE_FLAGS ${warnings})

target_include_directories(unit_tests_backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plugins
    ${PROJECT_SOURCE_DIR}/plugins
    ${PROJECT_SOURCE_DIR}
)

target_include_directories(unit_tests_backend
    SYSTEM
    PRIVATE
    ${PROJECT_SOURCE_DIR}/backend/src/generated
)

target_link_libraries(unit_tests_backend
    mavsdk_server
    mavsdk_action
    mavsdk_camera
    mavsdk_info
    mavsdk_mission
    mavsdk_offboard
    mavsdk_telemetry
    gRPC::grpc++
    gtest
    gmock
    gmock_main
)

if (BUILD_STATIC_MAVSDK_SERVER)
    target_link_options(unit_tests_backend PRIVATE -static-libgcc -static-libstdc++ -static)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set_target_properties(unit_tests_backend PROPERTIES LINK_SEARCH_START_STATIC ON)
    set_target_properties(unit_tests_backend PROPERTIES LINK_SEARCH_END_STATIC ON)
endif()

add_test(unit_tests unit_tests_backend)
