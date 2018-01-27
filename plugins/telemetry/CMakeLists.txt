add_library(dronecore_telemetry SHARED
    telemetry.cpp
    telemetry_impl.cpp
    math_conversions.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/core
    SYSTEM ${CMAKE_SOURCE_DIR}/libs/include
)

if(ANDROID)
    target_link_libraries(dronecore_telemetry
        log
    )
endif()

target_link_libraries(dronecore_telemetry
    dronecore
)

install(FILES
    telemetry.h
    DESTINATION ${dronecore_install_include_dir}
)

install(TARGETS dronecore_telemetry
    #EXPORT dronecore-targets
    DESTINATION ${dronecore_install_lib_dir}
)
