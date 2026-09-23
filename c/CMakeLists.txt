cmake_minimum_required(VERSION 3.22.1)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "CMAKE_BUILD_TYPE is not set, so this build is unoptimized. "
                   "Pass -DCMAKE_BUILD_TYPE=RelWithDebInfo for a release build.")
endif()
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

option(BUNDLE_STATIC_RUNTIME "Statically link libgcc, libstdc++, and libatomic into the shared library" OFF)
option(BUILD_EXAMPLES "Build examples" ON)

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

