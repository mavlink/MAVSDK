#set(_libs
#    dronelink
#    pthread
#)

list(APPEND integration_tests
    hello_world
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
