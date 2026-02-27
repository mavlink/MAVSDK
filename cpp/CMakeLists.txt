cmake_minimum_required(VERSION 3.22.1)

option(SUPERBUILD "Build dependencies" ON)
option(BUILD_MAVSDK_SERVER "Build mavsdk_server" OFF)
option(BUILD_WITH_PROTO_REFLECTION "Build mavsdk_server with proto reflection" OFF)
option(BUILD_SHARED_LIBS "Build core as shared libraries instead of static ones" ON)
option(MAVLINK_DIALECT "MAVLink dialect. Default: ardupilotmega" "ardupilotmega")
option(BUILD_TESTING "Build tests" ON)
option(BUILD_FUZZ_TESTS "Build fuzz tests with libfuzzer" OFF)
option(BUILD_WITHOUT_CURL "Build without CURL" OFF)

message("Using cmake version: ${CMAKE_VERSION}")

if (NOT MAVLINK_DIALECT)
    set(MAVLINK_DIALECT "ardupilotmega")
endif()

execute_process(
    COMMAND git describe --abbrev=8 --dirty --always --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_STR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GITVER
)

if (NOT GITVER EQUAL "0")
    message(STATUS "No git version found. Falling back to empty version no")
    set(VERSION_STR "0.0.0")
endif()

message(STATUS "Version: ${VERSION_STR}")
STRING(REGEX REPLACE v\([0-9]+\).[0-9]+.[0-9]+.*$ \\1 MAVSDK_VERSION_MAJOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.\([0-9]+\).[0-9]+.*$ \\1 MAVSDK_VERSION_MINOR "${VERSION_STR}")
STRING(REGEX REPLACE v[0-9]+.[0-9]+.\([0-9]+\).*$ \\1 MAVSDK_VERSION_PATCH "${VERSION_STR}")

message(STATUS "Version major: ${MAVSDK_VERSION_MAJOR}")
message(STATUS "Version minor: ${MAVSDK_VERSION_MINOR}")
message(STATUS "Version patch: ${MAVSDK_VERSION_PATCH}")

set(MAVSDK_SOVERSION_STRING ${MAVSDK_VERSION_MAJOR})
set(MAVSDK_VERSION_STRING ${MAVSDK_VERSION_MAJOR}.${MAVSDK_VERSION_MINOR}.${MAVSDK_VERSION_PATCH})

set(MAVLINK_XML_PATH "" CACHE PATH "Where the mavlink xml files are located.")

project(mavsdk_superbuild)

# FIXME: in the future we need to mark library methods that need to be visible manually
#        and remove CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.
#        At the moment, we are running into the 65535 object limit on Windows likely because of this.
if (BUILD_SHARED_LIBS AND MSVC AND BUILD_MAVSDK_SERVER)
    message(FATAL_ERROR
        "To build mavsdk_server on Windows, you need to use BUILD_SHARED_LIBS=OFF.\n"
        "This avoids the Windows 65535 object limit when statically linking all dependencies.\n"
        "Use: cmake -DBUILD_SHARED_LIBS=OFF ..."
    )
endif()

if (BUILD_BACKEND)
    message(FATAL_ERROR "The argument BUILD_BACKEND has been replaced by BUILD_MAVSDK_SERVER. To build mavsdk_server, use -DBUILD_MAVSDK_SERVER=ON.")
endif()

include(GNUInstallDirs)

add_subdirectory(third_party)
add_subdirectory(src)
