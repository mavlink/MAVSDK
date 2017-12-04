cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_INSTALL_PREFIX "../install" CACHE PATH "default cache path")

project(dronecore)

# Add DEBUG define for Debug target
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# This finds thread libs on Linux, Mac, and Windows.
find_package(Threads REQUIRED)

if(NOT MSVC)
    # Clang and GCC
    # We want C++11 and lots of warnings.
    # We are not using exceptions to make it easier to write wrappers.
    add_definitions(
        -std=c++11
        -fno-exceptions
    )

    set(warnings "-Wall -Wextra -Werror -Wshadow -Wno-strict-aliasing -Wold-style-cast -Wdouble-promotion -Wformat=2")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6)
            set(warnings "${warnings} -Wduplicated-cond -Wnull-dereference")
        endif()
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7)
            set(warnings "${warnings} -Wduplicated-branches")
        endif()

        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
            set(warnings "${warnings} -Wno-missing-field-initializers")
        endif()

        set(warnings "${warnings} -Wuseless-cast -Wlogical-op")
    endif()

    set(curl_lib "curl")

else()
    # MSBuild
    add_definitions(
        -std=c++11
        -DWINDOWS
        -DCURL_STATICLIB
    )
    set(CMAKE_CXX_FLAGS_RELEASE "/MD")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd")

    set(warnings "-WX -W2")

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
    # Some time ago we needed `-stdlib=libc++` but that does not seem to be true anymore.
    set(warnings "${warnings} -Wno-missing-braces")
endif()


# We need a define if on APPLE
if(APPLE)
    add_definitions("-DAPPLE")
endif()


if (ANDROID)
    add_definitions(-frtti)
    # Workaround for a plugin where the __androidx86__ is necessary.
    if (ANDROID_ABI STREQUAL "x86")
        add_definitions(-D__androidx86__)
    endif()
endif()

# Add definition for __FILENAME__ so we don't need to use __FILE__ because it also
# contains the path.
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

# Set these in a module to add a library.
set (additional_includes "")
set (additional_libs "")

# Header files in include denote public facing header files, the rest is in
# core or the respective plugin directories.
include_directories(
    .
    include
    libs/include
    core
    ${plugins}
    ${external_plugins}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/core
    SYSTEM ${additional_includes}
)

if (DEFINED additional_includes AND NOT additional_includes STREQUAL "")
    message(STATUS "Additional includes: ${additional_includes}")
endif()

if (ANDROID)
    include_directories(curl-android-ios/prebuilt-with-ssl/android/include)
endif()

if (IOS)
    include_directories(curl-android-ios/prebuilt-with-ssl/ios/include)
endif()

if (IOS OR ANDROID OR MSVC OR APPLE)
    set(library_type "STATIC")

    # We need tinyxml2 for the camera definition parsing.
    # We use the submodule when linking statically.
    add_subdirectory(libs/tinyxml2 EXCLUDE_FROM_ALL)
    include_directories(SYSTEM libs/tinyxml2)

else()
    set(library_type "SHARED")
endif()

# We build one static library.
add_library(dronecore ${library_type}
    core/global_include.cpp
    core/connection.cpp
    core/device.cpp
    core/mavlink_parameters.cpp
    core/mavlink_commands.cpp
    core/dronecore.cpp
    core/dronecore_impl.cpp
    core/mavlink_channels.cpp
    core/mavlink_receiver.cpp
    core/serial_connection.cpp
    core/tcp_connection.cpp
    core/udp_connection.cpp
    core/plugin_base.cpp
    core/plugin_impl_base.cpp
    core/curl_wrapper.cpp
    core/http_loader.cpp
    core/timeout_handler.cpp
    core/call_every_handler.cpp
    ${plugin_source_files}
)

target_link_libraries(dronecore
    ${CMAKE_THREAD_LIBS_INIT}
    ${curl_lib}
    tinyxml2
)
if (MSVC)
    target_link_libraries(dronecore
        ws2_32
    )
endif()

# cmake should check for C++11
set_property(TARGET dronecore PROPERTY CXX_STANDARD 11)
set_property(TARGET dronecore PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(dronecore
    PROPERTIES COMPILE_FLAGS ${warnings}
)

# Use tinyxml2 from the host system for Linux.
if(NOT IOS AND NOT ANDROID AND NOT MSVC AND NOT APPLE)
endif()

# We support install in order to use the header and library files in
# other applications.
if(ANDROID)
    set(lib_path "lib/android/${ANDROID_ABI}")
elseif(IOS)
    set(lib_path "lib/ios")
else()
    set(lib_path "lib")
endif()

install(TARGETS dronecore
    EXPORT dronecore-targets
    DESTINATION ${lib_path}
)

install(FILES
    include/dronecore.h
    core/device.h
    ${plugin_header_paths}
    DESTINATION "include/dronecore"
)

# TODO: enable again
if(NOT IOS AND NOT ANDROID AND false)
    add_subdirectory(integration_tests)

    if (DEFINED EXTERNAL_DIR AND NOT EXTERNAL_DIR STREQUAL "")
        add_subdirectory(${EXTERNAL_DIR}/integration_tests
            ${CMAKE_CURRENT_BINARY_DIR}/${EXTERNAL_DIR}/integration_tests)
    endif()

    enable_testing()

    add_subdirectory(libs/gtest EXCLUDE_FROM_ALL)

    # SYSTEM because we don't want warnings for gtest headers.
    include_directories(SYSTEM libs/gtest/googletest/include)
    include_directories(SYSTEM libs/gtest/googlemock/include)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTESTING")

    add_executable(dronecore-unittests
        core/global_include_test.cpp
        core/mavlink_channels_test.cpp
        core/unittests_main.cpp
        core/http_loader_test.cpp
        core/timeout_handler_test.cpp
        core/call_every_handler_test.cpp
        ${plugin_unittest_source_files}
        ${unit_tests_src}
    )

    if (MSVC)
        # We need this to prevent linking errors from happening in the Windows build.
        target_compile_definitions(dronecore-unittests PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
        target_compile_options(dronecore-unittests PUBLIC "/wd4251" "/wd4275")
    endif()

    target_compile_definitions(dronecore-unittests PRIVATE FAKE_TIME=1)

    set_target_properties(dronecore-unittests
        PROPERTIES COMPILE_FLAGS ${warnings}
    )

    target_link_libraries(dronecore-unittests
        dronecore
        gtest
        gtest_main
        gmock
        ${additional_libs}
    )

    add_test(unit_tests
        dronecore-unittests
    )

    # `make test` does not show output, but `make check` does
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

    # This includes all GTests that run integration tests
    add_executable(dronecore-integrationtests
        core/unittests_main.cpp
        ${integration_tests_src}
    )

    set_target_properties(dronecore-integrationtests
        PROPERTIES COMPILE_FLAGS ${warnings}
    )

    target_link_libraries(dronecore-integrationtests
        dronecore
        gtest
        gtest_main
        gmock
        ${additional_libs}
    )

    if (MSVC)
        # We need this to prevent linking errors from happening in the Windows build.
        target_compile_definitions(dronecore-integrationtests PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
        target_compile_options(dronecore-integrationtests PUBLIC "/wd4251" "/wd4275")
    endif()

    add_test(integration_tests
        dronecore-integrationtests
    )

    add_custom_command(TARGET dronecore-integrationtests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/start_px4_sitl.sh
            ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_command(TARGET dronecore-integrationtests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/stop_px4_sitl.sh
            ${CMAKE_CURRENT_BINARY_DIR}
    )

    set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage")

    if (CMAKE_BUILD_DRONECORESERVER)
        message(STATUS "Building dronecore server")
        add_subdirectory(grpc/server)
    else()
        message(STATUS "BUILD_DRONECORESERVER not set: not building dronecore server")
    endif()
endif()

if (DROP_DEBUG EQUAL 1)
    add_definitions(-DDROP_DEBUG=${DROP_DEBUG})

    add_executable(drop_debug
        debug_helpers/drop_debug_main.cpp
    )

    target_link_libraries(drop_debug
        dronecore
    )
endif()
