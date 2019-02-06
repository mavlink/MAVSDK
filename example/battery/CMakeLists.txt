cmake_minimum_required(VERSION 2.8.12)

project(battery)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
    # Line below required if /usr/local/include is not in your default includes
    #include_directories(/usr/local/include)
    # Line below required if /usr/local/lib is not in your default linker path
    #link_directories(/usr/local/lib)
    include_directories(/usr/local/include/dronecode_sdk/include/)
else()
    add_definitions("-std=c++11 -WX -W2")
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
endif()

add_executable(battery
    battery.cpp
)

target_link_libraries(battery
    dronecode_sdk
    dronecode_sdk_telemetry
    dronecode_sdk_mavlink_passthrough
)

