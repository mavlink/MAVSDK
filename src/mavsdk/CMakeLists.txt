add_library(mavsdk)

cmake_policy(SET CMP0076 NEW)

add_subdirectory(core)
add_subdirectory(plugins)

hunter_add_package(jsoncpp)
find_package(jsoncpp REQUIRED)

find_package(libevents REQUIRED)

target_link_libraries(mavsdk
    PUBLIC
    ${CMAKE_DL_LIBS}
    PRIVATE
    JsonCpp::JsonCpp
    rapidxml::rapidxml
    LibEvents::LibEvents
)

target_compile_definitions(mavsdk
    PRIVATE
    RAPIDXML_NO_EXCEPTIONS=1
)

if (NOT APPLE AND NOT ANDROID AND NOT MSVC)
    target_link_libraries(mavsdk
        PRIVATE
        stdc++fs
    )
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
