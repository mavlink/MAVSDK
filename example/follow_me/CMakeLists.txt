cmake_minimum_required(VERSION 2.8.12)

project(follow_me)

if(NOT MSVC)
    add_definitions("-std=c++11 -Wall -Wextra -Werror")
	# Line below required if /usr/local/include is not in your default includes
    #include_directories(/usr/local/include)
    # Line below required if /usr/local/lib is not in your default linker path
    #link_directories(/usr/local/lib)
    find_package(Boost 1.58 COMPONENTS system REQUIRED )
else()
    include_directories(${CMAKE_SOURCE_DIR}/../../install/include)
    link_directories(${CMAKE_SOURCE_DIR}/../../install/lib)
    add_definitions("-std=c++11 -WX -W2")
    find_package(Boost 1.64 COMPONENTS system date_time REQUIRED )
    # This is to avoid auto-linking of libraries that we don't build
    add_definitions("-DBOOST_ALL_NO_LIB")
endif()

if (MSVC)

else()

endif()

include_directories(${Boost_INCLUDE_DIR})

add_executable(follow_me
    follow_me.cpp
    fake_location_provider.cpp
)

target_link_libraries(follow_me
    LINK_PUBLIC ${Boost_LIBRARIES}
    dronecode_sdk
    dronecode_sdk_action
    dronecode_sdk_follow_me
    dronecode_sdk_telemetry
)


///
cmake_minimum_required(VERSION 2.8.12)

project(fly_qgc_mission)



add_executable(fly_qgc_mission
    fly_qgc_mission.cpp
)

target_link_libraries(fly_qgc_mission
    dronecode_sdk
    dronecode_sdk_action
    dronecode_sdk_mission
    dronecode_sdk_telemetry
)
