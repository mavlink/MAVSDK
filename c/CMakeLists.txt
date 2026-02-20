cmake_minimum_required(VERSION 3.22.1)
project(cmavsdk
    VERSION 3.10.2
    DESCRIPTION "C API wrapper for MAVSDK"
    LANGUAGES C CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(example)

