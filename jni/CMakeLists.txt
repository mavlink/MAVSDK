cmake_minimum_required(VERSION 3.10.2)
project(mavsdk-jni CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Generated plugin sources (from generate_from_protos.bash)
include(${CMAKE_CURRENT_SOURCE_DIR}/plugins_generated.cmake)

set(CORE_SOURCES
    mavsdk_jni.cpp
    configuration_jni.cpp
    log_jni.cpp
    system_jni.cpp
)

if(ANDROID)
    # Android build
    find_library(log-lib log)

    # Import prebuilt cmavsdk library
    add_library(cmavsdk SHARED IMPORTED)
    set_target_properties(
        cmavsdk
        PROPERTIES IMPORTED_LOCATION
        "${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libcmavsdk.so"
    )

    # Build JNI wrapper
    add_library(mavsdk_jni SHARED
        ${CORE_SOURCES}
        ${PLUGIN_SOURCES}
    )

    target_link_libraries(mavsdk_jni
        cmavsdk
        ${log-lib}
    )
else()
    # Desktop build (macOS/Linux)
    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})

    # Find prebuilt cmavsdk library
    set(CMAVSDK_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs" CACHE PATH "Path to libcmavsdk")

    # Build JNI wrapper
    add_library(mavsdk_jni SHARED
        ${CORE_SOURCES}
        ${PLUGIN_SOURCES}
    )

    # Link against cmavsdk
    if(APPLE)
        find_library(CMAVSDK_LIB cmavsdk PATHS ${CMAVSDK_LIB_DIR} NO_DEFAULT_PATH)
        target_link_libraries(mavsdk_jni
            ${JNI_LIBRARIES}
            ${CMAVSDK_LIB}
        )
    elseif(UNIX)
        target_link_libraries(mavsdk_jni
            ${JNI_LIBRARIES}
            ${CMAVSDK_LIB_DIR}/libcmavsdk.so
        )
    endif()
endif()
