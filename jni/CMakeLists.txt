cmake_minimum_required(VERSION 3.10.2)
project(mavsdk-jni CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/plugins_generated.cmake)

set(CORE_SOURCES
    mavsdk_jni.cpp
    configuration_jni.cpp
    log_jni.cpp
    system_jni.cpp
)

find_package(cmavsdk REQUIRED)

add_library(mavsdk_jni SHARED
    ${CORE_SOURCES}
    ${PLUGIN_SOURCES}
)

target_link_libraries(mavsdk_jni CMAVSDK::CMAVSDK)

if(ANDROID)
    target_link_libraries(mavsdk_jni log)

    target_link_options(mavsdk_jni PRIVATE -static-libstdc++)

    target_link_options(mavsdk_jni PRIVATE "-Wl,--exclude-libs,ALL")
    target_compile_options(mavsdk_jni PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
else()
    find_package(JNI REQUIRED)
    target_include_directories(mavsdk_jni PRIVATE ${JNI_INCLUDE_DIRS})
    target_link_libraries(mavsdk_jni ${JNI_LIBRARIES})
endif()

install(TARGETS mavsdk_jni
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib
)
