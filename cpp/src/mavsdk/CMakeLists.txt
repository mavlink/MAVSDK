add_library(mavsdk)

cmake_policy(SET CMP0076 NEW)

add_subdirectory(core)
add_subdirectory(plugins)

find_package(jsoncpp REQUIRED)

find_package(tinyxml2 REQUIRED)

find_package(libevents REQUIRED)
find_package(picosha2 REQUIRED)
find_package(mav REQUIRED)

target_link_libraries(mavsdk
    PUBLIC
    ${CMAKE_DL_LIBS}
    PRIVATE
    JsonCpp::JsonCpp
    tinyxml2::tinyxml2
    LibEvents::LibEvents
    mav::mav
)

if (NOT APPLE AND NOT ANDROID AND NOT MSVC
    AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9))
    target_link_libraries(mavsdk
        PRIVATE
        stdc++fs
    )
endif()

set(UNIT_TEST_SOURCES ${UNIT_TEST_SOURCES} PARENT_SCOPE)
