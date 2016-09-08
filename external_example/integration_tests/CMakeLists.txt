set(_libs
    dronelink
    pthread
    atomic
)

if (IOS)
    set(_libs
        ${_libs}
        libstdc++
    )
endif()

add_executable(hello_world
    hello_world_main.cpp
)

target_link_libraries(hello_world
    ${_libs}
)
