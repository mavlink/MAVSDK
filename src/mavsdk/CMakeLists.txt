add_library(mavsdk)

cmake_policy(SET CMP0076 NEW)

add_subdirectory(core)
add_subdirectory(plugins)

hunter_add_package(jsoncpp)
find_package(jsoncpp REQUIRED)

hunter_add_package(tinyxml2)
find_package(tinyxml2 REQUIRED)

find_package(libevents REQUIRED)

target_link_libraries(mavsdk
    PRIVATE
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
    LibEvents::LibEvents
)

if (NOT APPLE AND NOT ANDROID AND NOT MSVC)
    target_link_libraries(mavsdk
        PRIVATE
        stdc++fs
    )
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
