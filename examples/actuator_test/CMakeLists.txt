cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(actuator_test)

add_executable(actuator_test
    actuator_test.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(actuator_test
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(actuator_test PRIVATE -Wall -Wextra)
else()
    add_compile_options(actuator_test PRIVATE -WX -W2)
endif()

if(MINGW OR MSVC)
    # For M_PI
    add_target_definitions(actuator_test "-D_USE_MATH_DEFINES")
endif()
