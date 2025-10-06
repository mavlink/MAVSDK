add_executable(takeoff-and-land)

target_sources(takeoff-and-land
    PRIVATE
        takeoff-and-land.cpp
)

target_link_libraries(takeoff-and-land
    PRIVATE
        cmavsdk
)

add_executable(calibrate)

target_sources(calibrate
    PRIVATE
        calibrate.cpp
)

target_link_libraries(calibrate
    PRIVATE
        cmavsdk
)
