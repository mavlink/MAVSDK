cmake_minimum_required(VERSION 2.8.12)

project(dronelink)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Weffc++ -Werror")

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

file(GLOB plugins plugins/*)

# Look for plugins in plugin directory
foreach(plugin ${plugins})
	if(IS_DIRECTORY ${plugin})
        message("Found plugin ${plugin}")
		add_subdirectory(${plugin})
        foreach(source_file ${source_files})
            list(APPEND plugin_source_files "${plugin}/${source_file}")
        endforeach()
        list(APPEND plugin_class_names "${class_name}")
        list(APPEND plugin_header_files "${header_files}")
        list(APPEND plugin_impl_header_files "${impl_header_files}")
	endif()
endforeach()

set(INCLUDES_STRING "")
foreach(header_file ${plugin_header_files})
    set(INCLUDES_STRING "${INCLUDES_STRING}#include \"${header_file}\"\n")
endforeach()

set(IMPL_INCLUDES_STRING "")
foreach(impl_header_file ${plugin_impl_header_files})
    set(IMPL_INCLUDES_STRING "${IMPL_INCLUDES_STRING}#include \"${impl_header_file}\"\n")
endforeach()

set(FORWARD_DECLARATION_STRING "")
set(PLUGIN_CTOR_STRING "")
set(PLUGIN_DTOR_STRING "")
set(PLUGIN_MEMBER_STRING "")
set(PLUGIN_GETTER_STRING "")
set(PLUGIN_LIST_APPEND_STRING "")

set(first "TRUE")
foreach(class_name ${plugin_class_names})
    string(TOLOWER ${class_name} class_name_lowercase)

    set(getter_name ${class_name_lowercase})
    set(member_name "_${class_name_lowercase}")
    set(impl_class_name "${class_name}Impl")
    set(impl_member_name "_${class_name_lowercase}_impl")

    if (NOT ${first})
        set(FORWARD_DECLARATION_STRING "${FORWARD_DECLARATION_STRING}\n")
    endif()

    set(FORWARD_DECLARATION_STRING
        "${FORWARD_DECLARATION_STRING}class ${impl_class_name};")

    if (NOT ${first})
        set(PLUGIN_GETTER_STRING "${PLUGIN_GETTER_STRING}\n")
    endif()

    set(PLUGIN_GETTER_STRING
        "${PLUGIN_GETTER_STRING}    const ${class_name} &${getter_name}() { return ${member_name}; }")

    if (NOT ${first})
        set(PLUGIN_MEMBER_STRING "${PLUGIN_MEMBER_STRING}\n")
    endif()

    set(PLUGIN_MEMBER_STRING
        "${PLUGIN_MEMBER_STRING}    ${impl_class_name} *${impl_member_name};\n")
    set(PLUGIN_MEMBER_STRING
        "${PLUGIN_MEMBER_STRING}    ${class_name} ${member_name};\n")

    set(PLUGIN_CTOR_STRING
        "${PLUGIN_CTOR_STRING}    ${impl_member_name}(new ${impl_class_name}()),\n")
    set(PLUGIN_CTOR_STRING
        "${PLUGIN_CTOR_STRING}    ${member_name}(${impl_member_name}),\n")

    if (NOT ${first})
        set(PLUGIN_DTOR_STRING "${PLUGIN_DTOR_STRING}\n")
    endif()

    set(PLUGIN_DTOR_STRING
        "${PLUGIN_DTOR_STRING}    delete ${impl_member_name};")

    if (NOT ${first})
        set(PLUGIN_LIST_APPEND_STRING "${PLUGIN_LIST_APPEND_STRING}\n")
    endif()

    set(PLUGIN_LIST_APPEND_STRING
        "${PLUGIN_LIST_APPEND_STRING}   _plugin_impl_list.push_back(${impl_member_name});")

    set(first "FALSE")
endforeach()

configure_file(src/device_plugin_container.h.in src/device_plugin_container.h)
configure_file(src/device_plugin_container.cpp.in src/device_plugin_container.cpp)


include_directories(
    include
    libs/include
    src
    ${plugins}
)

add_library(dronelink STATIC
    src/connection.cpp
    src/device.cpp
    src/device_impl.cpp
    src/dronelink.cpp
    src/dronelink_impl.cpp
    src/error_handling.cpp
    src/mavlink_channels.cpp
    src/mavlink_receiver.cpp
    src/serial_connection.cpp
    src/tcp_connection.cpp
    src/udp_connection.cpp
    src/math_conversions.cpp
    src/plugin_impl_base.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/device_plugin_container.cpp
    ${plugin_source_files}
)

include_directories(
    include
    libs/include
    src
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

if(NOT IOS)
    add_subdirectory(integration_tests)

    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(unit_tests
        src/mavlink_channels_test.cpp
        src/unittests_main.cpp
    )

    target_link_libraries(unit_tests
        dronelink
        ${GTEST_BOTH_LIBRARIES}
        pthread
        atomic
    )

    add_test(tests
        unit_tests
    )

    # `make test` does not show output, but `make check` does
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
endif()
