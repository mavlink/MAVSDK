cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(battery)

add_executable(publish_battery
publish_battery.cpp
)

add_executable(subscribe_battery
    subscribe_battery.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(publish_battery
    MAVSDK::mavsdk
)

target_link_libraries(subscribe_battery
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(battery PRIVATE -Wall -Wextra)
else()
    add_compile_options(battery PRIVATE -W2)
endif()
