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
#include <mutex>

using namespace mavsdk;

struct Attitude {
    mutable std::mutex mutex {};
    float roll_deg {0.0f};
    float pitch_deg {0.0f};
    float yaw_deg {0.0f};
};

struct AttitudeSetpoint {
    mutable std::mutex mutex {};
    float roll_deg {0.0f};
    float pitch_deg {0.0f};
    float yaw_deg {0.0f};
};

static void usage(const std::string& bin_name);
static bool request_gimbal_device_information(MavlinkPassthrough& mavlink_passthrough);
static float degrees(float radians);
static float radians(float radians);

static constexpr auto test_prefix = "[TEST] ";

class Sender
{
public:
    explicit Sender(
            MavlinkPassthrough& mavlink_passthrough,
            AttitudeSetpoint &attitude_setpoint) :
        _mavlink_passthrough(mavlink_passthrough),
        _attitude_setpoint(attitude_setpoint),
        _thread(&Sender::run, this)
    {}

    ~Sender()
    {
        _should_exit = true;
        _thread.join();
    }

    void run()
    {
        while (!_should_exit) {
            send_autopilot_state_for_gimbal_device();
            send_gimbal_device_set_attitude();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

private:
    void send_autopilot_state_for_gimbal_device()
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
            _mavlink_passthrough.get_our_sysid(),
            _mavlink_passthrough.get_our_compid(),
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
        _mavlink_passthrough.send_message(message);
    }

    void send_gimbal_device_set_attitude()
    {
        const uint16_t flags =
            GIMBAL_DEVICE_FLAGS_ROLL_LOCK |
            GIMBAL_DEVICE_FLAGS_PITCH_LOCK;

        float q[4];
        {
            std::lock_guard<std::mutex> lock(_attitude_setpoint.mutex);
            mavlink_euler_to_quaternion(
                radians(_attitude_setpoint.roll_deg),
                radians(_attitude_setpoint.pitch_deg),
                radians(_attitude_setpoint.yaw_deg),
                q);
        }

        mavlink_message_t message;
        mavlink_msg_gimbal_device_set_attitude_pack(
            _mavlink_passthrough.get_our_sysid(),
            _mavlink_passthrough.get_our_compid(),
            &message,
            0, // broadcast
            0, // broadcast
            flags,
            q,
            NAN, // angular velocity X
            NAN, // angular velocity Y
            NAN // angular velocity Z
        );

        _mavlink_passthrough.send_message(message);
    }

    MavlinkPassthrough& _mavlink_passthrough;
    AttitudeSetpoint& _attitude_setpoint;
    std::thread _thread;
    std::atomic<bool> _should_exit {false};
};

static void subscribe_to_gimbal_device_attitude_status(MavlinkPassthrough& mavlink_passthrough, Attitude& attitude);

static bool test_device_information(MavlinkPassthrough& mavlink_passthrough);
static bool test_pitch(const Attitude& attitude, AttitudeSetpoint& attitude_setpoint);
static bool test_yaw_follow(MavlinkPassthrough& mavlink_passthrough, const Attitude& attitude);
static bool test_yaw_lock(MavlinkPassthrough& mavlink_passthrough, const Attitude& attitude);
static bool test_pitch_yaw(const std::string& description, float pitch_deg, float yaw_deg, const Attitude& attitude, AttitudeSetpoint& attitude_setpoint);

int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    std::string connection_url;
    ConnectionResult connection_result;

    std::cout << test_prefix << "Connecting... " << std::flush;

    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cout << "FAIL\n";
        std::cout << "-> connection failed: " << connection_result << std::endl;
        return 1;
    }

    {
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        mavsdk.register_on_discover([&prom](uint64_t /* uuid*/) {
            prom.set_value();
        });

        if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
            std::cout << "FAIL\n";
            std::cout << "-> no device found" << std::endl;
            return 1;
        }
        mavsdk.register_on_discover(nullptr);
    }

    System& system = mavsdk.system();
    MavlinkPassthrough mavlink_passthrough(system);

    if (!test_device_information(mavlink_passthrough)) {
        return 1;
    }

    Attitude attitude {};
    subscribe_to_gimbal_device_attitude_status(mavlink_passthrough, attitude);

    AttitudeSetpoint attitude_setpoint {};
    Sender sender(mavlink_passthrough, attitude_setpoint);

    if (!test_pitch(attitude, attitude_setpoint)) {
        return 1;
    }

    if (!test_yaw_follow(mavlink_passthrough, attitude)) {
        return 1;
    }

    if (!test_yaw_lock(mavlink_passthrough, attitude)) {
        return 1;
    }

    return 0;
}

bool test_device_information(MavlinkPassthrough& mavlink_passthrough)
{
    std::cout << test_prefix << "Requests gimbal device information... " << std::flush;

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
        std::cout << "FAIL\n";
        std::cout << "-> could not request gimbal device information" << std::endl;
        return false;
    }

    if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
        std::cout << "FAIL\n";
        std::cout << "-> no gimbal device information received" << std::endl;
        return false;
    }

    std::cout << "PASS" << std::endl;
    return true;
}

bool test_pitch(const Attitude& attitude, AttitudeSetpoint& attitude_setpoint)
{
    return test_pitch_yaw("Look forward", 0.0f, 0.0f, attitude, attitude_setpoint) &&
        test_pitch_yaw("Tilt 45 degrees down", -45.0f, 0.0f, attitude, attitude_setpoint) &&
        test_pitch_yaw("Tilt 90 degrees down", -90.0f, 0.0f, attitude, attitude_setpoint) &&
        test_pitch_yaw("Tilt 20 degrees up", 20.0f, 0.0f, attitude, attitude_setpoint) &&
        test_pitch_yaw("Look forward again", 0.0f, 0.0f, attitude, attitude_setpoint);
}

bool test_yaw_follow(MavlinkPassthrough& mavlink_passthrough, const Attitude& attitude)
{
    return true;
}

bool test_yaw_lock(MavlinkPassthrough& mavlink_passthrough, const Attitude&  attitude)
{
    return true;
}

bool test_pitch_yaw(const std::string& description, float pitch_deg, float yaw_deg, const Attitude& attitude, AttitudeSetpoint& attitude_setpoint)
{
    std::cout << test_prefix << description << "... " << std::flush;
    {
        std::lock_guard<std::mutex> lock(attitude_setpoint.mutex);
        attitude_setpoint.pitch_deg = pitch_deg;
        attitude_setpoint.yaw_deg = yaw_deg;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    const float margin_deg = 2.0f;

    std::lock_guard<std::mutex> lock(attitude.mutex);

    bool pitch_fail = false;
    bool yaw_fail = false;

    if (attitude.pitch_deg > pitch_deg + margin_deg) {
        pitch_fail = true;
    } else if (attitude.pitch_deg < pitch_deg - margin_deg) {
        pitch_fail = true;
    }

    if (attitude.yaw_deg > yaw_deg + margin_deg) {
        yaw_fail = true;
    } else if (attitude.yaw_deg < yaw_deg - margin_deg) {
        yaw_fail = true;
    }

    if (pitch_fail || yaw_fail) {
        std::cout << "FAIL\n";
    } else {
        std::cout << "PASS\n";
    }

    if (pitch_fail) {
        std::cout << "-> pitch is " << attitude.pitch_deg << " deg instead of " << pitch_deg << " deg\n";
    }

    if (yaw_fail) {
        std::cout << "-> yaw is " << attitude.yaw_deg << " deg instead of " << yaw_deg << " deg\n";
    }

    return !(pitch_fail || yaw_fail);
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

void subscribe_to_gimbal_device_attitude_status(MavlinkPassthrough& mavlink_passthrough, Attitude& attitude)
{
    mavlink_passthrough.subscribe_message_async(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS,
        [&attitude](const mavlink_message_t& message) {
        mavlink_gimbal_device_attitude_status_t attitude_status;
        mavlink_msg_gimbal_device_attitude_status_decode(&message, &attitude_status);

        float roll_rad, pitch_rad, yaw_rad;
        mavlink_quaternion_to_euler(attitude_status.q, &roll_rad, &pitch_rad, &yaw_rad);

        std::lock_guard<std::mutex> lock(attitude.mutex);
        attitude.roll_deg = degrees(roll_rad);
        attitude.pitch_deg = degrees(pitch_rad);
        attitude.yaw_deg = degrees(yaw_rad);
    });
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
