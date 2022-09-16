add_library(mavsdk)

cmake_policy(SET CMP0076 NEW)

add_subdirectory(core)
add_subdirectory(plugins)

hunter_add_package(jsoncpp)
find_package(jsoncpp REQUIRED)

hunter_add_package(tinyxml2)
find_package(tinyxml2 REQUIRED)

target_link_libraries(mavsdk
    PRIVATE
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
)

# TODO
set_property(SOURCE module.modulemap
    PROPERTY MACOSX_PACKAGE_LOCATION "Modules")

# TODO
set_target_properties(mavsdk PROPERTIES
    FRAMEWORK TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_NAME_DIR @rpath
    PUBLIC_HEADER mavsdk_server_api.h
    #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
    MACOSX_FRAMEWORK_INFO_PLIST ${PROJECT_SOURCE_DIR}/mavsdk_server/cmake/MacOSXFrameworkInfo.plist.in
)

if (NOT APPLE AND NOT ANDROID AND NOT MSVC)
    target_link_libraries(mavsdk
        PRIVATE
        stdc++fs
    )
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
