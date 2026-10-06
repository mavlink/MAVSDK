cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mavlink_direct_receiver_custom)

find_package(MAVSDK REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(receiver_custom
    receiver_custom.cpp
)

target_link_libraries(receiver_custom
    PRIVATE
    MAVSDK::mavsdk
    nlohmann_json::nlohmann_json
)

if(NOT MSVC)
    target_compile_options(receiver_custom PRIVATE -Wall -Wextra)
else()
    target_compile_options(receiver_custom PRIVATE -W2)
endif()
