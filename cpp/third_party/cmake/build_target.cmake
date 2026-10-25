include(ProcessorCount)

function(build_target TARGET_NAME)
    set(TARGET_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${TARGET_NAME}")
    set(TARGET_BINARY_DIR "${DEPS_BUILD_PATH}/${TARGET_NAME}")
    set(TARGET_INSTALL_DIR "${DEPS_INSTALL_PATH}")

    file(MAKE_DIRECTORY ${TARGET_BINARY_DIR})

    if(DEFINED CMAKE_GENERATOR_PLATFORM AND NOT "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "")
        set(PLATFORM_ARGUMENT "-A${CMAKE_GENERATOR_PLATFORM}")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND}
            "-G${CMAKE_GENERATOR}"
            "${PLATFORM_ARGUMENT}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX:PATH=${TARGET_INSTALL_DIR}"
            "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"
            "-DPLATFORM=${PLATFORM}" # for iOS toolchain
            "-DDEPLOYMENT_TARGET=${DEPLOYMENT_TARGET}" # for iOS toolchain
            "-DENABLE_STRICT_TRY_COMPILE=${ENABLE_STRICT_TRY_COMPILE}" # for iOS toolchain
            "-DMAVLINK_DIALECT=${MAVLINK_DIALECT}"
            "-DMAVLINK_URL=${MAVLINK_URL}"
            "-DMAVLINK_HASH=${MAVLINK_HASH}"
            "-DLIBMAVLIKE_URL=${LIBMAVLIKE_URL}"
            "-DLIBMAVLIKE_HASH=${LIBMAVLIKE_HASH}"
            "${TARGET_SOURCE_DIR}"
        WORKING_DIRECTORY "${TARGET_BINARY_DIR}"
        RESULT_VARIABLE CONFIGURE_FAILED
    )

    if(${CONFIGURE_FAILED})
        message(FATAL_ERROR "${TARGET_SOURCE_DIR} failed to configure!")
    endif()

    ProcessorCount(NUM_PROCS)
    if(NUM_PROCS EQUAL 0)
        set(NUM_PROCS 4)
    endif()
    set(ENV{MAKEFLAGS} -j${NUM_PROCS})

    if(MSVC)
        # MSBuild ignores MAKEFLAGS. Build projects in parallel, including the nested
        # ExternalProject builds, and compile the files within a project in parallel.
        set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} ${NUM_PROCS})
        string(FIND "$ENV{CL}" "/MP" CL_HAS_MP)
        if(CL_HAS_MP EQUAL -1)
            set(ENV{CL} "/MP $ENV{CL}")
        endif()

        execute_process(COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
            WORKING_DIRECTORY ${TARGET_BINARY_DIR}
            RESULT_VARIABLE BUILD_FAILED
        )
    else()
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
            WORKING_DIRECTORY ${TARGET_BINARY_DIR}
            RESULT_VARIABLE BUILD_FAILED
        )
    endif()

    if(${BUILD_FAILED})
        message(FATAL_ERROR "${TARGET_BINARY_DIR} failed to build!")
    endif()
endfunction()
