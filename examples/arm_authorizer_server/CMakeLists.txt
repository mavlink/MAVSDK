cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(arm_authorizer_server)

add_executable(arm_authorizer_server
        arm_authorizer_server.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(arm_authorizer_server
        MAVSDK::mavsdk
)

if (NOT MSVC)
    add_compile_options(arm_authorizer_server PRIVATE -Wall -Wextra)
else ()
    add_compile_options(arm_authorizer_server PRIVATE -W2)
endif ()
