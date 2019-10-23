cmake_minimum_required(VERSION 2.8.12)

project(ftp_client)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra")
else()
    add_definitions("-std=c++11 -WX -W2")
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
