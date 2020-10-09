include(CMakeFindDependencyMacro)

if(NOT ON)
    if(${CMAKE_VERSION} VERSION_LESS "3.9.0")
        find_package(CURL REQUIRED CONFIG)
    else()
        find_dependency(CURL REQUIRED CONFIG)
    endif()

    find_dependency(jsoncpp)
    find_dependency(tinyxml2)

    if(OFF)
        find_dependency(gRPC)
    endif()
endif()

get_filename_component(MAVSDK_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include("${MAVSDK_CMAKE_DIR}/MAVSDKTargets.cmake")
