include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

add_subdirectory(action)
add_subdirectory(gimbal)
add_subdirectory(mission)
add_subdirectory(offboard)
add_subdirectory(telemetry)
add_subdirectory(logging)
add_subdirectory(info)
add_subdirectory(follow_me)
