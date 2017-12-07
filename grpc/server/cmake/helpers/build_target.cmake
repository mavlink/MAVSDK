function(build_target SRC_DIR TARGET_DIR)
    file(MAKE_DIRECTORY ${TARGET_DIR}/build)

    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -G${CMAKE_GENERATOR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            ${SRC_DIR}
        WORKING_DIRECTORY ${TARGET_DIR}/build
    )

    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${TARGET_DIR}/build
    )
endfunction()
