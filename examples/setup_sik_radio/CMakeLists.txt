cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(setup_sik_radio)

add_executable(setup_sik_radio
    setup_sik_radio.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(setup_sik_radio
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(setup_sik_radio PRIVATE -Wall -Wextra)
else()
    add_compile_options(setup_sik_radio PRIVATE -WX -W2)
endif()
