include(FetchContent)

FetchContent_Declare(
    libevents
    GIT_REPOSITORY https://github.com/mavlink/libevents.git
    GIT_TAG 7c1720749dfe555ec2e71d5f9f753e6ac1244e1c
)

FetchContent_GetProperties(libevents)
if(NOT libevents_POPULATED)
    FetchContent_Populate(libevents)
endif()

add_subdirectory(${libevents_SOURCE_DIR}/libs/cpp ${libevents_BINARY_DIR})

# The upstream libevents only sets INSTALL_INTERFACE. Headers are under
# libs/cpp/ but included as <libevents/...>, so copy them into
# include_wrapper/libevents/ and add that as BUILD_INTERFACE.
set(_libevents_include_wrapper "${libevents_BINARY_DIR}/include_wrapper")
file(COPY "${libevents_SOURCE_DIR}/libs/cpp/"
     DESTINATION "${_libevents_include_wrapper}/libevents"
     FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

target_include_directories(libevents INTERFACE
    $<BUILD_INTERFACE:${_libevents_include_wrapper}>
)
