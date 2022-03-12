add_library(mavsdk)

cmake_policy(SET CMP0076 NEW)


#=============================================================================
# generate mavlink headers
#
add_custom_target(
   mav_gen
   COMMAND ${SDK_SOURCE_DIR}/mavlink_generator/createSDKHeader ${SDK_SOURCE_DIR}
   COMMENT "Generating Mavlink Headers"
   VERBATIM
)


hunter_add_package(jsoncpp)
find_package(jsoncpp REQUIRED)

hunter_add_package(tinyxml2)
find_package(tinyxml2 REQUIRED)

target_link_libraries(mavsdk
    PRIVATE
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
)

if (NOT APPLE AND NOT ANDROID AND NOT MSVC)
    target_link_libraries(mavsdk
        PRIVATE
        stdc++fs
    )
endif()


target_include_directories(mavsdk SYSTEM
    PRIVATE ../third_party/mavlink/include
)

add_dependencies(mavsdk mav_gen)

add_subdirectory(core)
add_subdirectory(plugins)


set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
