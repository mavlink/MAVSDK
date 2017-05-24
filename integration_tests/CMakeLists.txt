#set(_libs
#    dronelink
#    pthread
#)
#if (IOS)
#    set(_libs
#        ${_libs}
#        libstdc++
#    )
#endif()

list(APPEND integration_tests
    simple_connect
    async_connect
    telemetry_simple
    telemetry_async
    telemetry_modes
    telemetry_health
    simple_hover
    async_hover
    takeoff_and_kill
    offboard_velocity
    logging
    info
    curl
)


foreach(name ${integration_tests})
    # This can be used to produce individual executables
    #add_executable(${name}
    #    ${name}_main.cpp
    #    ${name}.cpp
    #)
    #
    #target_link_libraries(${name}
    #    ${_libs}
    #)

    list(APPEND integration_tests_src ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp)
endforeach()

# These are consumed in ../CMakeLists.txt
set(integration_tests_src ${integration_tests_src} PARENT_SCOPE)
