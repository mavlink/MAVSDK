//
// Test against a MAVLink gimbal device according to the gimbal protocol v2.
//
// Author: Julian Oes <julian@oes.ch>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>

using namespace mavsdk;

static void usage(const std::string& bin_name);
static bool request_gimbal_device_information(MavlinkPassthrough& mavlink_passthrough);
static float degrees(float radians);
static float radians(float radians);

static void send_autopilot_state_for_gimbal_device(MavlinkPassthrough& mavlink_passthrough);
static  void send_gimbal_device_set_attitude(MavlinkPassthrough& mavlink_passthrough);

int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    std::string connection_url;
    ConnectionResult connection_result;

    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    {
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        std::cout << "Waiting to discover system..." << std::endl;
        mavsdk.register_on_discover([&prom](uint64_t /* uuid*/) {
            prom.set_value();
        });

        if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
            std::cerr << "No device found, exiting." << std::endl;
            return 1;
        }
        mavsdk.register_on_discover(nullptr);
    }

    System& system = mavsdk.system();
    MavlinkPassthrough mavlink_passthrough(system);

    {
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        mavlink_passthrough.subscribe_message_async(
            MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION,
            [&prom, &mavlink_passthrough](const mavlink_message_t& /* message */) {
                // We only need it once.
                mavlink_passthrough.subscribe_message_async(MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION, nullptr);
                prom.set_value();
        });

        if (!request_gimbal_device_information(mavlink_passthrough)) {
            std::cerr << "Could not get gimbal device information." << std::endl;
            return 1;
        }

        if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
            std::cerr << "No device information received, exiting." << std::endl;
            return 1;
        }
        std::cout << "Received gimbal device information." << std::endl;
    }

    mavlink_passthrough.subscribe_message_async(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS,
        [](const mavlink_message_t& message) {
        mavlink_gimbal_device_attitude_status_t attitude_status;
        mavlink_msg_gimbal_device_attitude_status_decode(&message, &attitude_status);

        float roll, pitch, yaw;
        mavlink_quaternion_to_euler(attitude_status.q, &roll, &pitch, &yaw);

        std::cout << "roll: " << degrees(roll) << ", pitch: " << degrees(pitch) << ", yaw: " << degrees(yaw) << "\n";
    });


    while (true) {
        send_autopilot_state_for_gimbal_device(mavlink_passthrough);
        send_gimbal_device_set_attitude(mavlink_passthrough);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}

bool request_gimbal_device_information(MavlinkPassthrough& mavlink_passthrough)
{
    MavlinkPassthrough::CommandLong command;
    command.command = MAV_CMD_REQUEST_MESSAGE;
    command.param1 = static_cast<float>(MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION);
    command.target_sysid = mavlink_passthrough.get_target_sysid();
    command.target_compid = mavlink_passthrough.get_target_compid();

    return (mavlink_passthrough.send_command_long(command) == MavlinkPassthrough::Result::Success);
}

void usage(const std::string& bin_name)
{
    std::cout << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

float degrees(float radians)
{
    return radians * 180.0f / static_cast<float>(M_PI);
}

float radians(float degrees)
{
    return degrees / 180.0f * static_cast<float>(M_PI);
}

void send_autopilot_state_for_gimbal_device(MavlinkPassthrough& mavlink_passthrough)
{
    const float q[4] {1, 0, 0, 0};

    const uint16_t estimator_status =
        ESTIMATOR_ATTITUDE |
        ESTIMATOR_VELOCITY_HORIZ |
        ESTIMATOR_VELOCITY_VERT |
        ESTIMATOR_POS_HORIZ_REL |
        ESTIMATOR_POS_HORIZ_ABS |
        ESTIMATOR_POS_VERT_ABS |
        ESTIMATOR_POS_VERT_AGL |
        ESTIMATOR_PRED_POS_HORIZ_REL |
        ESTIMATOR_PRED_POS_HORIZ_ABS;

    mavlink_message_t message;
    mavlink_msg_autopilot_state_for_gimbal_device_pack(
        mavlink_passthrough.get_our_sysid(),
        mavlink_passthrough.get_our_compid(),
        &message,
        0, // broadcast
        0, // broadcast
        0, // FIXME: time us
        q,
        0, // q estimated delay
        0.0f, // vx
        0.0f, // vy
        0.0f, // vz
        0, // estimated delay
        0.0f, // feed forward angular velocity z
        estimator_status,
        MAV_LANDED_STATE_IN_AIR);
    mavlink_passthrough.send_message(message);
}

void send_gimbal_device_set_attitude(MavlinkPassthrough& mavlink_passthrough)
{
    const uint16_t flags =
        GIMBAL_DEVICE_FLAGS_ROLL_LOCK |
        GIMBAL_DEVICE_FLAGS_PITCH_LOCK;

    float roll_deg = 0.0f;
    float pitch_deg = -45.0f;
    float yaw_deg = 20.0f;

    float q[4];
    float roll_rad = radians(roll_deg);
    float pitch_rad = radians(pitch_deg);
    float yaw_rad = radians(yaw_deg);

    mavlink_euler_to_quaternion(roll_rad, pitch_rad, yaw_rad, q);

    mavlink_message_t message;
    mavlink_msg_gimbal_device_set_attitude_pack(
        mavlink_passthrough.get_our_sysid(),
        mavlink_passthrough.get_our_compid(),
        &message,
        0, // broadcast
        0, // broadcast
        flags,
        q,
        NAN, // angular velocity X
        NAN, // angular velocity Y
        NAN // angular velocity Z
    );

    mavlink_passthrough.send_message(message);
}
