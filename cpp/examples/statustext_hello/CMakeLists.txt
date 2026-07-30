cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(statustext_hello)

add_executable(statustext_hello
    statustext_hello.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(statustext_hello
    MAVSDK::mavsdk
)

if (NOT MSVC)
    add_compile_options(statustext_hello PRIVATE -Wall -Wextra)
else ()
    add_compile_options(statustext_hello PRIVATE -W2)
endif ()
