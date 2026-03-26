find_package(MAVSDK REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/plugins_generated.cmake)

set(CORE_SOURCES
    cmavsdk/log.cpp
    cmavsdk/mavsdk.cpp
    cmavsdk/system.cpp
)

set(CORE_HEADERS
    cmavsdk/autopilot.h
    cmavsdk/component_type.h
    cmavsdk/connection_result.h
    cmavsdk/log.h
    cmavsdk/mavsdk.h
    cmavsdk/mavsdk_export.h
    cmavsdk/system.h
    cmavsdk/vehicle.h
)

add_library(cmavsdk)

target_sources(cmavsdk
    PRIVATE
        ${CORE_SOURCES}
        ${CORE_HEADERS}
        ${PLUGIN_HEADERS}
        ${PLUGIN_SOURCES}
)

target_include_directories(cmavsdk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(cmavsdk
    PRIVATE
        MAVSDK::mavsdk
)

target_compile_definitions(cmavsdk
    PRIVATE
        CMAVSDK_BUILD
)

if(MSVC)
    target_compile_options(cmavsdk PRIVATE /W3)
else()
    target_compile_options(cmavsdk PRIVATE -Wall -Wextra -fvisibility=hidden)
endif()

if(BUNDLE_STATIC_RUNTIME)
    if(NOT (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND UNIX AND NOT APPLE))
        message(WARNING "BUNDLE_STATIC_RUNTIME is only supported on Linux with GCC/Clang")
    endif()

    target_link_options(cmavsdk PRIVATE
        -static-libgcc
        -static-libstdc++
    )
    target_link_libraries(cmavsdk PRIVATE atomic.a)
endif()

set_target_properties(cmavsdk PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 3
    EXPORT_NAME CMAVSDK
)

install(TARGETS cmavsdk
    EXPORT cmavsdkTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY cmavsdk/
    DESTINATION include/cmavsdk
    FILES_MATCHING PATTERN "*.h"
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmavsdkConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmavsdkConfig.cmake
    INSTALL_DESTINATION lib/cmake/cmavsdk
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmavsdkConfig.cmake
    DESTINATION lib/cmake/cmavsdk
)

install(EXPORT cmavsdkTargets
    FILE cmavsdkTargets.cmake
    NAMESPACE CMAVSDK::
    DESTINATION lib/cmake/cmavsdk
)

