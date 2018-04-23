include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/third_party/mavlink/include
)

if(IOS OR ANDROID OR APPLE)
    set(PLUGIN_LIBRARY_TYPE STATIC)
else()
    set(PLUGIN_LIBRARY_TYPE SHARED)
endif()

add_subdirectory(action)
add_subdirectory(gimbal)
add_subdirectory(mission)
add_subdirectory(offboard)
add_subdirectory(telemetry)
add_subdirectory(logging)
add_subdirectory(info)
add_subdirectory(follow_me)
add_subdirectory(camera)

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
