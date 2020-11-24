cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ftp_client)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-Wall -Wextra")
else()
    add_definitions("-WX -W2")
endif()

find_package(MAVSDK REQUIRED)

add_executable(ftp_client
    ftp_client.cpp
)

target_link_libraries(ftp_client
    MAVSDK::mavsdk_ftp
    MAVSDK::mavsdk
    ${CMAKE_THREAD_LIBS_INIT}
)
