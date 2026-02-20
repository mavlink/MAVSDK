add_executable(takeoff-and-land)

target_sources(takeoff-and-land
    PRIVATE
        takeoff-and-land.c
)

target_link_libraries(takeoff-and-land
    PRIVATE
        cmavsdk
)

add_executable(calibrate)

target_sources(calibrate
    PRIVATE
        calibrate.c
)

target_link_libraries(calibrate
    PRIVATE
        cmavsdk
)

add_executable(camera-client)

target_sources(camera-client
    PRIVATE
        camera-client.c
)

target_link_libraries(camera-client
    PRIVATE
        cmavsdk
)

add_executable(camera-server)

target_sources(camera-server
    PRIVATE
        camera-server.c
)

target_link_libraries(camera-server
    PRIVATE
        cmavsdk
)

add_executable(mission)

target_sources(mission
    PRIVATE
        mission.c
)

target_link_libraries(mission
    PRIVATE
        cmavsdk
)

add_executable(params)

target_sources(params
    PRIVATE
        params.c
)

target_link_libraries(params
    PRIVATE
        cmavsdk
)

