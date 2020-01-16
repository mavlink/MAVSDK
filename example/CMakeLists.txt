cmake_minimum_required(VERSION 2.8.12)
project(mavsdk_examples)

# This allows to build all examples at once.

add_subdirectory(battery)
add_subdirectory(calibrate)
add_subdirectory(fly_mission)
add_subdirectory(fly_multiple_drones)
add_subdirectory(fly_qgc_mission)
add_subdirectory(follow_me)
add_subdirectory(geofence_inclusion)
add_subdirectory(mavshell)
add_subdirectory(multiple_drones)
add_subdirectory(offboard_velocity)
add_subdirectory(takeoff_land)
add_subdirectory(transition_vtol_fixed_wing)
add_subdirectory(mavlink_ftp_client)
add_subdirectory(mavlink_ftp_server)
