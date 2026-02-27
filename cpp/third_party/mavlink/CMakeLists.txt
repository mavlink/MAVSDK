include(FetchContent)

set(MAVLINK_HASH d6a7eeaf43319ce6da19a1973ca40180a4210643)
set(MAVLINK_URL "https://github.com/mavlink/mavlink")
set(MAVLINK_DIALECT ${MAVLINK_DIALECT} CACHE STRING "MAVLink dialect to generate")

FetchContent_Declare(
    mavlink
    GIT_REPOSITORY ${MAVLINK_URL}
    GIT_TAG ${MAVLINK_HASH}
)
FetchContent_GetProperties(mavlink)
if(NOT mavlink_POPULATED)
    FetchContent_Populate(mavlink)
    if(IOS)
        execute_process(
            COMMAND "git apply ${PROJECT_SOURCE_DIR}/mavlink_ios.patch"
            WORKING_DIRECTORY ${mavlink_SOURCE_DIR}
        )
    endif()
endif()

add_subdirectory(${mavlink_SOURCE_DIR} ${mavlink_BINARY_DIR})

target_include_directories(mavlink INTERFACE
    $<BUILD_INTERFACE:${mavlink_BINARY_DIR}/include>
)

set(MAVLINK_SOURCE_DIR ${mavlink_SOURCE_DIR} CACHE PATH "Where the MAVLink is located.")
