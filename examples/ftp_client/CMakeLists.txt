cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ftp_client)

add_executable(ftp_client
    ftp_client.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(ftp_client
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(ftp_client PRIVATE -Wall -Wextra)
else()
    add_compile_options(ftp_client PRIVATE -W2)
endif()
