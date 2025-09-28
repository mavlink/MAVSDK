find_package(MAVSDK REQUIRED)

set(C_API_SOURCES
    action/action.cpp
    log.cpp
    mavsdk.cpp
    system.cpp
    telemetry/telemetry.cpp
)

set(C_API_HEADERS
    autopilot.h
    action/action.h
    component_type.h
    connection_result.h
    log.h
    mavsdk.h
    mavsdk_export.h
    system.h
    telemetry/telemetry.h
    vehicle.h
)

add_library(cmavsdk)

target_sources(cmavsdk
    PRIVATE
        ${C_API_SOURCES}
        ${C_API_HEADERS}
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
        #-Werror
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
    FILES
        ${C_API_HEADERS}
    DESTINATION include/cmavsdk
)

install(EXPORT cmavsdkTargets
    FILE cmavsdkConfig.cmake
    NAMESPACE CMAVSDK::
    DESTINATION lib/cmake/cmavsdk
)

