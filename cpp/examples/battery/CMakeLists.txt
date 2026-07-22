cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(battery)

find_package(MAVSDK REQUIRED)
find_package(nlohmann_json QUIET)

if(nlohmann_json_FOUND)
    add_executable(publish_battery
        publish_battery.cpp
    )
    target_link_libraries(publish_battery
        MAVSDK::mavsdk
        nlohmann_json::nlohmann_json
    )
else()
    message(STATUS "nlohmann_json not found, skipping publish_battery example")
endif()

add_executable(subscribe_battery
    subscribe_battery.cpp
)

target_link_libraries(subscribe_battery
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(battery PRIVATE -Wall -Wextra)
else()
    add_compile_options(battery PRIVATE -W2)
endif()
