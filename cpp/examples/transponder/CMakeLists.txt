cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(transponder)

find_package(MAVSDK REQUIRED)

add_executable(transponder
    transponder.cpp
)

target_link_libraries(transponder
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(transponder PRIVATE -Wall -Wextra)
else()
    add_compile_options(transponder PRIVATE -W2)
endif()
