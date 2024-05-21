cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(logfile_download)

add_executable(logfile_download
    logfile_download.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(logfile_download
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(logfile_download PRIVATE -Wall -Wextra)
else()
    add_compile_options(logfile_download PRIVATE -W2)
endif()
