cmake_minimum_required(VERSION 2.8.12)

project(calibrate)

find_package(Threads REQUIRED)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
    # Line below required if /usr/local/include is not in your default includes
    #include_directories(/usr/local/include)
    # Line below required if /usr/local/lib is not in your default linker path
    #link_directories(/usr/local/lib)
else()
    add_definitions("-std=c++11 -WX -W2")
endif()

add_executable(calibrate
    calibrate.cpp
)

target_link_libraries(calibrate
    dronecode_sdk_calibration
    dronecode_sdk
    ${CMAKE_THREAD_LIBS_INIT}
)

