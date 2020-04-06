//
// Dummy gimbal manager implementation written using MavlinkPassthrough.
// Used to test the gimbal_manager, gimbal_device, and gimbal plugins.
//
// Author: Kalyan Sriram <coder.kalyan@gmail.com>

#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <sys/time.h>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console color

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url> <id>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev:[baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl
              << "Id should be from [1-6] indicating gimbal 1-6" << std::endl;
}

uint8_t parse_component_id(char* raw) {
    switch (std::stoi(raw)) {
        case 1:
            return MAV_COMP_ID_GIMBAL;
        case 2:
            return MAV_COMP_ID_GIMBAL2;
        case 3:
            return MAV_COMP_ID_GIMBAL3;
        case 4:
            return MAV_COMP_ID_GIMBAL4;
        case 5:
            return MAV_COMP_ID_GIMBAL5;
        case 6:
            return MAV_COMP_ID_GIMBAL6;
        default:
            return MAV_COMP_ID_GIMBAL;
    }
}

int main(int argc, char** argv)
{
    Mavsdk dc;
    uint8_t manager_sid = 1, manager_cid = MAV_COMP_ID_PATHPLANNER;

    Mavsdk::Configuration config(manager_cid, manager_sid);
    dc.set_configuration(config);

    std::string connection_url;
    uint8_t device_id;
    ConnectionResult connection_result;

    bool discovered_system = false;
    if (argc == 3) {
        connection_url = argv[1];
        connection_result = dc.add_any_connection(connection_url);
        device_id = parse_component_id(argv[2]);
    } else {
        usage(argv[0]);
        return 1;
    }

    std::cout << "Configured for gimbal component " << device_id
              << "(" << std::stoi(argv[2]) << ")" << std::endl;

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Connection failed: " << connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    System& system = dc.system();

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&discovered_system](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discovered_system = true;
    });

    sleep_for(seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    mavlink_gimbal_device_information_t gimbal_device_info;
    bool received_information = false;
    bool discovered = false;
    int manager_flags = GIMBAL_MANAGER_CAP_FLAGS_ENUM_END;

    // Here's our actual listening code
    MavlinkPassthrough pass(system);

    struct timeval tv_start;
    struct timeval tv_status_last;
    struct timeval tv_hb_last;
    struct timeval tv;
    unsigned long time_in_mils;
    unsigned long time_status_last;
    unsigned long time_hb_last;
    unsigned long time_start;

    // Discover our gimbal device
    pass.subscribe_message_async(MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION,
        [&received_information, &gimbal_device_info, &pass](const mavlink_message_t message) {
        mavlink_msg_gimbal_device_information_decode(&message, &gimbal_device_info);
        received_information = true;
    });

    // Listen for commands
    pass.subscribe_message_async(MAVLINK_MSG_ID_COMMAND_LONG,
        [&received_information, &discovered, &device_id, &manager_sid, &manager_cid, &pass, &gimbal_device_info, &time_in_mils, &time_start](const mavlink_message_t message) {

        mavlink_command_long_t cmd;
        mavlink_msg_command_long_decode(&message, &cmd);

        uint16_t command_types[] = { MAV_CMD_DO_GIMBAL_MANAGER_ATTITUDE,
                                     MAV_CMD_DO_SET_ROI_LOCATION,
                                     MAV_CMD_DO_SET_ROI_WPNEXT_OFFSET,
                                     MAV_CMD_DO_SET_ROI_SYSID,
                                     MAV_CMD_DO_SET_ROI_NONE,
                                     MAV_CMD_DO_GIMBAL_MANAGER_TRACK_POINT,
                                     MAV_CMD_DO_GIMBAL_MANAGER_TRACK_RECTANGLE };
        const char* command_names[] = { "MAV_CMD_DO_GIMBAL_MANAGER_ATTITUDE",
                                        "MAV_CMD_DO_SET_ROI_LOCATION",
                                        "MAV_CMD_DO_SET_ROI_WPNEXT_OFFSET",
                                        "MAV_CMD_DO_SET_ROI_SYSID",
                                        "MAV_CMD_DO_SET_ROI_NONE",
                                        "MAV_CMD_DO_GIMBAL_MANAGER_TRACK_POINT",
                                        "MAV_CMD_DO_GIMBAL_MANAGER_TRACK_RECTANGLE" };
        short unsigned int* command_index = std::find(command_types, command_types+7, cmd.command);

        if (cmd.command == MAV_CMD_REQUEST_MESSAGE) {
            std::cout << "Received request for gimbal manager info!" << std::endl;
            if (cmd.param1 == MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION) {
                // Respond to requests with gimbal manager info

                // If we're already discovered, don't respond
                if (discovered) return;

                if (!received_information) {
                    std::cout << "Gimbal device info not yet received, sending in progress message" << std::endl;
                    mavlink_message_t ack;
                    mavlink_msg_command_ack_pack(manager_sid, manager_cid, &ack,
                        MAV_CMD_REQUEST_MESSAGE, MAV_RESULT_IN_PROGRESS, 0, 0, 0, 0);

                    pass.send_message(ack);
                } else {
                    mavlink_message_t ack;
                    mavlink_msg_command_ack_pack(manager_sid, manager_cid, &ack,
                        MAV_CMD_REQUEST_MESSAGE, MAV_RESULT_ACCEPTED, 0, 0, 0, 0);
                    pass.send_message(ack);

                    mavlink_message_t info;
                    mavlink_msg_gimbal_manager_information_pack(manager_sid, manager_cid,
                            &info,
                            time_in_mils - time_start,
                            manager_sid, device_id,
                            gimbal_device_info.tilt_max, gimbal_device_info.tilt_min,
                            gimbal_device_info.tilt_rate_max,
                            gimbal_device_info.pan_max, gimbal_device_info.pan_min,
                            gimbal_device_info.pan_rate_max);
                    pass.send_message(info);
                    std::cout << "Sent gimbal info!" << std::endl;
                    discovered = true;
                }
            }
        } else if (command_index != command_types+7) {
            // Respond to various gimbal manager commands
            // Since this is a dummy we just print an acknowledgement

            // We only want to respond to requests for our gimbal device; if it's
            // not ours, ignore
            // Until SET_ATTITUDE gets a proper device id field we'll have to settle
            // for testing against component and system id
            if (cmd.target_system != manager_sid || cmd.target_component != manager_cid) return;

            std::cout << "Received " <<  command_names[command_index - command_types] << ", data:"
                      << cmd.param1 << ", "
                      << cmd.param2 << ", "
                      << cmd.param3 << ", "
                      << cmd.param4 << ", "
                      << cmd.param5 << ", "
                      << cmd.param6 << ", "
                      << cmd.param7 << std::endl;

            // Reply with COMMAND_ACK
            mavlink_message_t ack;
            mavlink_msg_command_ack_pack(manager_sid, manager_cid,
                    &ack,
                    cmd.command,
                    MAV_RESULT_ACCEPTED,
                    0, 0, message.sysid, message.compid);
            pass.send_message(ack);
        }
    });

    pass.subscribe_message_async(MAVLINK_MSG_ID_GIMBAL_MANAGER_SET_ATTITUDE,
        [&device_id, &manager_sid, &manager_cid, &pass](const mavlink_message_t message) {
        mavlink_gimbal_manager_set_attitude_t set_attitude_manager;
        mavlink_msg_gimbal_manager_set_attitude_decode(&message, &set_attitude_manager);

        // We only want to respond to requests for our gimbal device; if it's
        // not ours, ignore

        if (set_attitude_manager.flags >= 16) {
            // Override manager flags to 8 (default) if flags is greater than 16
            // (normally, we would implement 16+ in the gimbal manager control code)
            // (however, since this is a dummy manager, it doesn't make sense to)
            set_attitude_manager.flags = 8;
        }

        mavlink_message_t set_attitude_device;
        mavlink_msg_gimbal_device_set_attitude_pack(manager_sid, manager_cid,
                &set_attitude_device,
                manager_cid, device_id,
                set_attitude_manager.flags,
                set_attitude_manager.q,
                set_attitude_manager.angular_velocity_x,
                set_attitude_manager.angular_velocity_y,
                set_attitude_manager.angular_velocity_z);
        pass.send_message(set_attitude_device);

        std::cout << "Set attitude! ("
                  << set_attitude_manager.angular_velocity_x << ", "
                  << set_attitude_manager.angular_velocity_y << ", "
                  << set_attitude_manager.angular_velocity_z << ")" << std::endl;
    });

    gettimeofday(&tv_start, NULL);
    gettimeofday(&tv_status_last, NULL);
    gettimeofday(&tv_hb_last, NULL);
    time_start = 1000 * tv_start.tv_sec + (int) (tv_start.tv_usec * 0.001);

    // publish messages regularly
    while (true) {
        gettimeofday(&tv, NULL);
        time_in_mils = 1000 * tv.tv_sec + (int) (tv.tv_usec * 0.001);
        time_status_last = 1000 * tv_status_last.tv_sec + (int) (tv_status_last.tv_usec * 0.001);
        time_hb_last = 1000 * tv_hb_last.tv_sec + (int) (tv_hb_last.tv_usec * 0.001);

        // Send GIMBAL_MANAGER_STATUS messages at a rate of 5hz (every 200 ms)
        if (time_in_mils - time_status_last > 200) {
            mavlink_message_t status_message;
            mavlink_msg_gimbal_manager_status_pack(manager_sid, manager_cid,
                    &status_message, time_in_mils - time_start, manager_flags);
            pass.send_message(status_message);
            gettimeofday(&tv_status_last, NULL);
        }

        // Send HEARTBEAT messages at a rate of 1hz (every 1000ms)
        if (time_in_mils - time_hb_last > 1000) {
            mavlink_message_t hb_message;
            mavlink_msg_heartbeat_pack(manager_sid, manager_cid,
                    &hb_message, MAV_TYPE_ONBOARD_CONTROLLER,
                    MAV_AUTOPILOT_INVALID,
                    0, 0, MAV_STATE_ACTIVE);
            pass.send_message(hb_message);

            // Request gimbal info at a rate of 1hz if we don't have it yet
            if (!received_information) {
                std::cout << "Requesting gimbal device info" << std::endl;
                mavlink_message_t request_message;
                mavlink_msg_command_long_pack(manager_sid, manager_cid,
                        &request_message,
                        manager_sid, device_id,
                        MAV_CMD_REQUEST_MESSAGE,
                        0,
                        MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION,
                        0, 0, 0, 0, 0, 0);
            }
            gettimeofday(&tv_hb_last, NULL);
        }

        sleep_for(milliseconds(50));
    }

    return 0;
}
