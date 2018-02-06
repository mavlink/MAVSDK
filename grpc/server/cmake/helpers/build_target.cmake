function(build_target SRC_DIR TARGET_DIR)
    file(MAKE_DIRECTORY ${TARGET_DIR}/build)

    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -G${CMAKE_GENERATOR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            "-DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}"
            "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DBINARY_DIR=${TARGET_DIR}
            -DANDROID_ABI=${ANDROID_ABI}
            -DANDROID_STL=${ANDROID_STL}
            -DIOS_PLATFORM=${IOS_PLATFORM}
            -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
            ${SRC_DIR}
        WORKING_DIRECTORY "${TARGET_DIR}/build"
    )

    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${TARGET_DIR}/build
    )
endfunction()
