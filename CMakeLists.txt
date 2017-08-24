cmake_minimum_required(VERSION 2.8.12)

project(dronecore)

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")


# We want C++11 and lots of warnings.
if(NOT MSVC)
    # Clang and GCC
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
    set(platform_libs "pthread")
    set(curl_lib "curl")
else()
    # MSBuild
    add_definitions("-std=c++11 -WX -W2 -DWINDOWS -DCURL_STATICLIB")
    set(platform_libs "Ws2_32.lib")
    # You need to call cmake with -DWIN_CURL_INCLUDE_DIR:STRING="C:\\curl-7.54.1\\include"
    if(NOT WIN_CURL_INCLUDE_DIR)
        message(FATAL_ERROR "Please provide argument -DWIN_CURL_INCLUDE_DIR:STRING=\"path_to_curl_include\"")
    endif()
    if(NOT WIN_CURL_LIB)
        message(FATAL_ERROR "Please provide argument -DWIN_CURL_LIBSTRING=\"path_to_curl_lib\"")
    endif()
    include_directories(${WIN_CURL_INCLUDE_DIR})
    set(curl_lib ${WIN_CURL_LIB})
endif()

# Clang needs this warning disabled.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions("-stdlib=libc++ -Wno-missing-braces")
endif()

if (ANDROID)
    add_definitions("-frtti")
endif()

if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
    add_definitions("-Wno-missing-field-initializers")
endif()

# Set these in a module to add a library.
set (additional_includes "")
set (additional_libs "")

# Autogenerate the device_plugin_container.{h|cpp} to include all plugins.
include(autogenerate_plugin_container.cmake)

# Header files in include denote public facing header files, the rest is in
# core or the respective plugin directories.
include_directories(
    include
    libs/include
    core
    ${plugins}
    ${external_plugins}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/core
    ${EXTERNAL_DIR}/${additional_includes}
)

if (ANDROID)
    include_directories(curl-android-ios/prebuilt-with-ssl/android/include)
endif()

# We build one static library.
add_library(dronecore STATIC
    core/global_include.cpp
    core/connection.cpp
    core/device.cpp
    core/device_impl.cpp
    core/mavlink_parameters.cpp
    core/mavlink_commands.cpp
    core/dronecore.cpp
    core/dronecore_impl.cpp
    core/mavlink_channels.cpp
    core/mavlink_receiver.cpp
    core/serial_connection.cpp
    core/tcp_connection.cpp
    core/udp_connection.cpp
    core/plugin_impl_base.cpp
    core/curl_wrapper.cpp
    core/http_loader.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/core/device_plugin_container.cpp
    ${plugin_source_files}
)

# We support install in order to use the header and static library files in
# other applications.
if(ANDROID)
    set(lib_path "lib/android/${ANDROID_ABI}")

    # The old armeabi arch doesn't support promises yet with gcc 4.9.
    if (ANDROID_ABI STREQUAL "armeabi")
        add_definitions(-DNO_PROMISES)
    endif()

elseif(IOS)
    set(lib_path "lib/ios")
else()
    set(lib_path "lib")
endif()

install(TARGETS dronecore
    DESTINATION ${lib_path}
)

install(FILES
    include/dronecore.h
    include/device.h
    core/plugin_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/device_plugin_container.h
    ${plugin_header_paths}
    DESTINATION "include/dronecore"
)

if(NOT IOS AND NOT ANDROID AND NOT PYTHON)
    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    enable_testing()

    add_subdirectory(libs/gtest EXCLUDE_FROM_ALL)
    include_directories(libs/gtest/googletest/include)
    include_directories(libs/gtest/googlemock/include)

    if(NOT MSVC)
        # gtest has these warnings
        add_definitions("-Wno-sign-compare")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTESTING")

    add_executable(unit_tests_runner
        core/global_include_test.cpp
        core/mavlink_channels_test.cpp
        core/unittests_main.cpp
        core/http_loader_test.cpp
        ${plugin_unittest_source_files}
        ${unit_tests_src}
    )

    target_link_libraries(unit_tests_runner
        dronecore
        gtest
        gtest_main
        ${platform_libs}
        ${curl_lib}
        gmock
        ${additional_libs}
    )

    add_test(unit_tests
        unit_tests_runner
    )

    # `make test` does not show output, but `make check` does
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

    # This includes all GTests that run integration tests
    add_executable(integration_tests_runner
        ${integration_tests_src}
    )

    target_link_libraries(integration_tests_runner
        dronecore
        gtest
        gtest_main
        ${platform_libs}
        ${curl_lib}
        ${additional_libs}
    )

    add_test(integration_tests
        integration_tests_runner
    )

    add_custom_command(TARGET integration_tests_runner
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/start_px4_sitl.sh
            ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_command(TARGET integration_tests_runner
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/stop_px4_sitl.sh
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

if (PYTHON)
    find_package(PythonLibs REQUIRED)
    message("Python include directory: ${PYTHON_INCLUDE_DIRS}")
    message("Python library: ${PYTHON_LIBRARIES}")
    include_directories(
        libs/pybind11/include
        ${PYTHON_INCLUDE_DIRS}
    )
    execute_process(COMMAND
        python -c "import site; site.getusersitepackages(); print(site.USER_SITE)"
        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    message("Python site-packages: ${PYTHON_SITE_PACKAGES}")
    # make sure PYTHON_MODULE_PREFIX always has a value
    if (NOT PYTHON_MODULE_PREFIX)
        execute_process(COMMAND
            python -c "from distutils import sysconfig; print(sysconfig.get_config_var('SO'))"
            OUTPUT_VARIABLE PYTHON_MODULE_EXTENSION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

        message("Python module extension: ${PYTHON_MODULE_EXTENSION}")
    endif()

    set(PYBIND11_CPP_STANDARD -std=c++11)
    add_subdirectory(libs/pybind11)
    add_library(pydronecore MODULE
        core/global_include.cpp
        core/connection.cpp
        core/device.cpp
        core/device_impl.cpp
        core/mavlink_parameters.cpp
        core/mavlink_commands.cpp
        core/dronecore.cpp
        core/dronecore_impl.cpp
        core/mavlink_channels.cpp
        core/mavlink_receiver.cpp
        core/serial_connection.cpp
        core/tcp_connection.cpp
        core/udp_connection.cpp
        core/plugin_impl_base.cpp
        core/curl_wrapper.cpp
        core/http_loader.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/core/device_plugin_container.cpp
        ${plugin_source_files}
    )

    set_target_properties(pydronecore PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}")
    set_target_properties(pydronecore PROPERTIES SUFFIX "${PYTHON_MODULE_EXTENSION}")
    target_link_libraries(pydronecore
        curl
        ${PYTHON_LIBRARIES}
    )

    install(FILES "${CMAKE_BINARY_DIR}/${PYTHON_MODULE_PREFIX}pydronecore${PYTHON_MODULE_EXTENSION}" DESTINATION ${PYTHON_SITE_PACKAGES})
endif()

if (DROP_DEBUG EQUAL 1)
    add_definitions(-DDROP_DEBUG=${DROP_DEBUG})

    add_executable(drop_debug
        debug_helpers/drop_debug_main.cpp
    )

    target_link_libraries(drop_debug
        dronecore
        ${platform_libs}
    )
endif()
