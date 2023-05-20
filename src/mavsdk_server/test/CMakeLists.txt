cmake_minimum_required(VERSION 3.10.2)

add_executable(unit_tests_mavsdk_server
    action_service_impl_test.cpp
    mavsdk_server_main.cpp
    camera_service_impl_test.cpp
    connection_initiator_test.cpp
    core_service_impl_test.cpp
    mission_service_impl_test.cpp
    offboard_service_impl_test.cpp
    telemetry_service_impl_test.cpp
    info_service_impl_test.cpp
)

set_target_properties(unit_tests_mavsdk_server PROPERTIES COMPILE_FLAGS ${warnings})

target_include_directories(unit_tests_mavsdk_server
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/../../mavsdk/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../mavsdk/plugins
)

target_include_directories(unit_tests_mavsdk_server
    SYSTEM
    PRIVATE
    ${PROJECT_SOURCE_DIR}/mavsdk_server/src/generated
)

target_link_libraries(unit_tests_mavsdk_server
    PRIVATE
    mavsdk
    mavsdk_server
    gRPC::grpc++
    gtest
    gmock
    gmock_main
)

if (BUILD_STATIC_MAVSDK_SERVER)
    target_link_options(unit_tests_mavsdk_server PRIVATE -static-libgcc -static-libstdc++ -static)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set_target_properties(unit_tests_mavsdk_server PROPERTIES LINK_SEARCH_START_STATIC ON)
    set_target_properties(unit_tests_mavsdk_server PROPERTIES LINK_SEARCH_END_STATIC ON)
endif()

if(NOT BUILD_STATIC_MAVSDK_SERVER AND ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"))
    target_link_libraries(unit_tests_mavsdk_server PRIVATE ${CMAKE_DL_LIBS})
endif()

add_test(unit_tests unit_tests_mavsdk_server)
