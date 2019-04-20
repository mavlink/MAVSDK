cmake_minimum_required(VERSION 2.8.12)

project(follow_me)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
    # Line below required if /usr/local/include is not in your default includes
    #include_directories(/usr/local/include)
    # Line below required if /usr/local/lib is not in your default linker path
    #link_directories(/usr/local/lib)
else()
    #include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    #link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
    add_definitions("-std=c++11 -WX -W2")
endif()

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

target_link_libraries(follow_me
    dronecode_sdk_action
    dronecode_sdk_follow_me
    dronecode_sdk_telemetry
    dronecode_sdk
    ${CMAKE_THREAD_LIBS_INIT}
)
