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

target_compile_options(cmavsdk
    PRIVATE
        -Wall
        -Wextra
        -fvisibility=hidden
)

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

install(EXPORT cmavsdkTargets
    FILE cmavsdkConfig.cmake
    NAMESPACE CMAVSDK::
    DESTINATION lib/cmake/cmavsdk
)

