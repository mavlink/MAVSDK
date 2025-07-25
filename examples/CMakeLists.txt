cmake_minimum_required(VERSION 3.10.2)
project(mavsdk_examples)

# This allows to build all examples at once.

if (WERROR)
    add_compile_options(-Werror)
endif()

add_subdirectory(arm_authorizer_server)
add_subdirectory(autopilot_server)
add_subdirectory(battery)
add_subdirectory(calibrate)
add_subdirectory(camera)
add_subdirectory(camera_server)
add_subdirectory(camera_settings)
add_subdirectory(camera_zoom)
add_subdirectory(component_metadata)
add_subdirectory(disconnect)
add_subdirectory(events)
add_subdirectory(reboot)
add_subdirectory(reconnect)
add_subdirectory(fly_mission)
add_subdirectory(fly_multiple_drones)
add_subdirectory(fly_qgc_mission)
add_subdirectory(follow_me)
add_subdirectory(mavlink_forwarding)
add_subdirectory(ftp_client)
add_subdirectory(ftp_server)
add_subdirectory(geofence)
add_subdirectory(gimbal)
add_subdirectory(gimbal_device_tester)
add_subdirectory(log_callback)
add_subdirectory(logfile_download)
add_subdirectory(log_streaming)
add_subdirectory(mavlink_direct)
add_subdirectory(mavshell)
add_subdirectory(multiple_drones)
add_subdirectory(offboard)
add_subdirectory(parachute)
add_subdirectory(params)
add_subdirectory(set_actuator)
add_subdirectory(sniffer)
add_subdirectory(system_info)
add_subdirectory(takeoff_and_land)
add_subdirectory(terminate)
add_subdirectory(transponder)
add_subdirectory(tune)
add_subdirectory(vtol_transition)
add_subdirectory(winch)
add_subdirectory(wind)
# Disabled, requires mavsdk_server library.
#add_subdirectory(start_stop_server)

if(NOT MSVC)
    # Requires sdl2, so we're skipping it on Windows.
    add_subdirectory(manual_control)
endif()
