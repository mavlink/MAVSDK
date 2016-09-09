set(_libs
    dronelink
    pthread
)

if (IOS)
    set(_libs
        ${_libs}
        libstdc++
    )
endif()

add_executable(simple_connect
    simple_connect_main.cpp
)

target_link_libraries(simple_connect
    ${_libs}
)

add_executable(simple_telemetry
    simple_telemetry_main.cpp
)

target_link_libraries(simple_telemetry
    ${_libs}
)

add_executable(simple_hover
    simple_hover_main.cpp
)

target_link_libraries(simple_hover
    ${_libs}
)
