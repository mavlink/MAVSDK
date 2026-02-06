cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tune)

add_executable(tune
    tune.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(tune
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(tune PRIVATE -Wall -Wextra)
else()
    add_compile_options(tune PRIVATE -W2)
endif()
