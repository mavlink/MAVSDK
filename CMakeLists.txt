cmake_minimum_required(VERSION 3.13)

option(HUNTER_ENABLED "Enable Hunter package manager support" OFF)
option(BUILD_MAVSDK_SERVER "Build mavsdk_server" OFF)
option(BUILD_WITH_PROTO_REFLECTION "Build mavsdk_server with proto reflection" OFF)
option(BUILD_SHARED_LIBS "Build core as shared libraries instead of static ones" ON)
option(MAVLINK_DIALECT "MAVLink dialect. Default: common" "common")
option(BUILD_WITHOUT_CURL "Build without curl" OFF)
option(BUILD_TESTING "Build tests" ON)

if (NOT MAVLINK_DIALECT)
    set(MAVLINK_DIALECT "common")
endif()

project(mavsdk_superbuild)

add_subdirectory(third_party)

set(DEPENDENCIES "mavlink" "jsoncpp" "tinyxml2" "liblzma")

if (NOT BUILD_WITHOUT_CURL)
    if (NOT IOS)
        list(APPEND DEPENDENCIES "zlib-ng")
    endif()
    list(APPEND DEPENDENCIES "curl")
endif()

if (BUILD_MAVSDK_SERVER)
    list(APPEND DEPENDENCIES "grpc")
endif()

if (BUILD_TESTING)
    list(APPEND DEPENDENCIES "gtest")
endif()

ExternalProject_add(main
    PREFIX main
    URL ${CMAKE_CURRENT_SOURCE_DIR}/src
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
	-DHUNTER_ENABLED=${HUNTER_ENABLED}
	-DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
	-DBUILD_MAVSDK_SERVER=${BUILD_MAVSDK_SERVER}
	-DBUILD_WITH_PROTO_REFLECTION=${BUILD_WITH_PROTO_REFLECTION}
	-DBUILD_WITHOUT_CURL=${BUILD_WITHOUT_CURL}
	-DMAVLINK_DIALECT=${MAVLINK_DIALECT}
	-DBUILD_TESTING=${BUILD_TESTING}
    DEPENDS ${DEPENDENCIES}
)
