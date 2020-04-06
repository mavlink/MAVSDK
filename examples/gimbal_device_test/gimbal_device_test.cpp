//
// Dummy gimbal device implementation written using MavlinkPassthrough.
// Used to test the gimbal_manager, gimbal_device, and gimbal plugins.
// NOTE: THE DEVICE MUST BE STARTED BEFORE THE MANAGER WILL RESPOND TO DISCOVERY REQUESTS.
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
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk dc;
    uint8_t gimbal_sid = 2, gimbal_cid = parse_component_id(argv[2]);
    bool discovered_system = false;
    std::string connection_url = argv[1];
    ConnectionResult connection_result;

    Mavsdk::Configuration config(gimbal_sid, gimbal_cid);
    dc.set_configuration(config);

    connection_result = dc.add_any_connection(connection_url);

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

    bool received_information = false;

    // Define some dummy gimbal info
    mavlink_gimbal_device_information_t gimbal_device_info;
    float angular_velocity_x = 0;
    float angular_velocity_y = 0;
    float angular_velocity_z = 0;
    float q[] = {0, 0, 0, 0};

    gimbal_device_info.vendor_name[32] = {0};
    gimbal_device_info.model_name[32] = {0};
    gimbal_device_info.firmware_version = 1;
    gimbal_device_info.cap_flags = GIMBAL_DEVICE_CAP_FLAGS_HAS_YAW_FOLLOW;
    gimbal_device_info.tilt_max = 3.14;
    gimbal_device_info.tilt_min = -3.14;
    gimbal_device_info.tilt_rate_max = 0.5;
    gimbal_device_info.pan_max = 3.14;
    gimbal_device_info.pan_min = -3.14;
    gimbal_device_info.pan_rate_max = 0.5;

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
        [&received_information, &gimbal_sid, &gimbal_cid, &pass, &gimbal_device_info, &time_in_mils, &time_start](const mavlink_message_t message) {
        // Respond to gimbal device info requests as part of discovery process
        mavlink_command_long_t cmd;
        mavlink_msg_command_long_decode(&message, &cmd);

        // If it's not addressed to us, don't respond
        if (cmd.target_system != gimbal_sid || cmd.target_component != gimbal_cid) return;

        if (cmd.command == MAV_CMD_REQUEST_MESSAGE) {
            std::cout << "Received request for gimbal device info!" << std::endl;
            if (cmd.param1 == MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION) {
                // Respond to requests with gimbal device info
                mavlink_message_t info;
                mavlink_msg_gimbal_device_information_pack(gimbal_sid, gimbal_cid,
                        &info,
                        time_in_mils - time_start,
                        gimbal_device_info.vendor_name,
                        gimbal_device_info.model_name,
                        gimbal_device_info.firmware_version,
                        gimbal_device_info.cap_flags,
                        gimbal_device_info.tilt_max,
                        gimbal_device_info.tilt_min,
                        gimbal_device_info.tilt_rate_max,
                        gimbal_device_info.pan_max,
                        gimbal_device_info.pan_min,
                        gimbal_device_info.pan_rate_max);

                pass.send_message(info);
                std::cout << "Sent gimbal info!" << std::endl;
            }
        }
    });

    pass.subscribe_message_async(MAVLINK_MSG_ID_GIMBAL_DEVICE_SET_ATTITUDE,
        [&gimbal_sid, &gimbal_cid, &angular_velocity_x, &angular_velocity_y, &angular_velocity_z, &q, &pass](const mavlink_message_t message) {
        // Receive SET_ATTITUDE command
        // we'll just store the angular velocity (because there is no physical gimbal to move)
        mavlink_gimbal_device_set_attitude_t set_attitude;
        mavlink_msg_gimbal_device_set_attitude_decode(&message, &set_attitude);

        // If it's not addressed to us, don't respond
        if (set_attitude.target_system != gimbal_sid || set_attitude.target_component != gimbal_cid) return;

        angular_velocity_x = set_attitude.angular_velocity_x;
        angular_velocity_y = set_attitude.angular_velocity_y;
        angular_velocity_z = set_attitude.angular_velocity_z;
        q[0] = set_attitude.q[0];
        q[1] = set_attitude.q[1];
        q[2] = set_attitude.q[2];
        q[3] = set_attitude.q[3];

        std::cout << "Set attitude! ("
                  << angular_velocity_x << ", "
                  << angular_velocity_y << ", "
                  << angular_velocity_z << ")" << std::endl;
    });

    pass.subscribe_message_async(MAVLINK_MSG_ID_AUTOPILOT_STATE_FOR_GIMBAL_DEVICE,
        [&gimbal_sid, &gimbal_cid, &pass](const mavlink_message_t message)
        {
        // Receive AUTOPILOT_STATE_FOR_GIMBAL_DEVICE; normally, we would use the
        // data in this message to control a gimbal, but as there is no physical
        // gimbal we will just print instead.

        mavlink_autopilot_state_for_gimbal_device_t state;
        mavlink_msg_autopilot_state_for_gimbal_device_decode(&message, &state);

        std::cout << "Received AUTOPILOT_STATE_FOR_GIMBAL_DEVICE! data: (vx:"
                  << state.vx << ", vy:"
                  << state.vy << ", vz:"
                  << state.vz << ")" << std::endl;
        }
    );

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

        // Send GIMBAL_DEVICE_ATTITUDE_STATUS messages at a rate of 5hz (every 200 ms)
        if (time_in_mils - time_status_last > 200) {
            mavlink_message_t status_message;
            mavlink_msg_gimbal_device_attitude_status_pack(gimbal_sid, gimbal_cid,
                    &status_message,
                    time_in_mils - time_start,
                    gimbal_device_info.cap_flags,
                    q,
                    angular_velocity_x,
                    angular_velocity_y,
                    angular_velocity_z,
                    0);
            pass.send_message(status_message);
            gettimeofday(&tv_status_last, NULL);
        }

        // Send HEARTBEAT messages at a rate of 1hz (every 1000ms)
        if (time_in_mils - time_hb_last > 1000) {
            mavlink_message_t hb_message;
            mavlink_msg_heartbeat_pack(gimbal_sid, gimbal_cid,
                    &hb_message, MAV_TYPE_GIMBAL,
                    MAV_AUTOPILOT_INVALID,
                    0, 0, MAV_STATE_ACTIVE);
            pass.send_message(hb_message);
            std::cout << "Heartbeat" << std::endl;

            gettimeofday(&tv_hb_last, NULL);
        }

        sleep_for(milliseconds(50));
    }

    return 0;
}
