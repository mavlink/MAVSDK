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

struct GimbalAttitude {
    mutable std::mutex mutex {};
    float roll_deg {NAN};
    float pitch_deg {NAN};
    float yaw_deg {NAN};
};

struct VehicleAttitude {
    mutable std::mutex mutex {};
    float roll_deg {0.0f};
    float pitch_deg {0.0f};
    float yaw_deg {0.0f};
};

enum class Mode {
    Follow,
    Lock
};

struct AttitudeSetpoint {
    mutable std::mutex mutex {};
    Mode mode {Mode::Follow};
    float roll_deg {0.0f};
    float pitch_deg {0.0f};
    float yaw_deg {0.0f};
};

static void usage(const std::string& bin_name);
static bool request_gimbal_device_information(MavlinkPassthrough& mavlink_passthrough);
static void subscribe_to_gimbal_device_attitude_status(MavlinkPassthrough& mavlink_passthrough, GimbalAttitude& gimbal_attitude);
static bool test_device_information(MavlinkPassthrough& mavlink_passthrough);
static bool wait_for_yaw_estimator_to_converge(const GimbalAttitude& gimbal_attitude);
static float degrees(float radians);
static float radians(float radians);

static constexpr auto test_prefix = "[TEST] ";

class Sender
{
public:
    explicit Sender(
            MavlinkPassthrough& mavlink_passthrough,
            AttitudeSetpoint& attitude_setpoint,
            VehicleAttitude& vehicle_attitude) :
        _mavlink_passthrough(mavlink_passthrough),
        _attitude_setpoint(attitude_setpoint),
        _vehicle_attitude(vehicle_attitude),
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
        float q[4];
        {
            std::lock_guard<std::mutex> lock(_vehicle_attitude.mutex);
            mavlink_euler_to_quaternion(
                    radians(_vehicle_attitude.roll_deg),
                    radians(_vehicle_attitude.pitch_deg),
                    radians(_vehicle_attitude.yaw_deg),
                    q);
        }

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
        uint16_t flags =
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

            if (_attitude_setpoint.mode == Mode::Lock) {
                flags |= GIMBAL_DEVICE_FLAGS_YAW_LOCK;
            }
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
    VehicleAttitude& _vehicle_attitude;
    std::thread _thread;
    std::atomic<bool> _should_exit {false};
};

class Tester
{
public:
    explicit Tester(AttitudeSetpoint& attitude_setpoint, VehicleAttitude& vehicle_attitude, GimbalAttitude& gimbal_attitude) :
        _attitude_setpoint(attitude_setpoint),
        _vehicle_attitude(vehicle_attitude),
        _gimbal_attitude(gimbal_attitude)
    {}

    bool test_pitch()
    {
        return test_pitch_yaw("Look forward", 0.0f, 0.0f, Mode::Follow) &&
            test_pitch_yaw("Tilt 45 degrees down", -45.0f, 0.0f, Mode::Follow) &&
            test_pitch_yaw("Tilt 90 degrees down", -90.0f, 0.0f, Mode::Follow) &&
            test_pitch_yaw("Tilt 20 degrees up", 20.0f, 0.0f, Mode::Follow) &&
            test_pitch_yaw("Look forward again", 0.0f, 0.0f, Mode::Follow);
    }

    bool test_yaw_follow()
    {
        if (!test_pitch_yaw("Switch to follow mode", 0.0f, 0.0f, Mode::Follow)) {
            return false;
        }

        std::cout << test_prefix << "Yaw 20 degrees to the right... ";
        {
            std::lock_guard<std::mutex> lock(_vehicle_attitude.mutex);
            _vehicle_attitude.yaw_deg = 20.0f;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "DONE\n";

        return test_pitch_yaw("Look to the right", 0.0f, 90.0f, Mode::Follow) &&
            test_pitch_yaw("Look 60 degrees to the left", 0.0f, -90.0f, Mode::Follow) &&
            test_pitch_yaw("Tilt 45 degrees down", -45.0f, -90.0f, Mode::Follow) &&
            test_pitch_yaw("Look 60 degrees to the right", -45.0f, 90.0f, Mode::Follow) &&
            test_pitch_yaw("Look forward again", 0.0f, 0.0f, Mode::Follow);
    }

    bool test_yaw_lock()
    {
        if (!test_pitch_yaw("Switch to lock mode", 0.0f, 0.0f, Mode::Follow)) {
            return false;
        }

        std::cout << test_prefix << "Yaw 30 degrees to the left... ";
        {
            std::lock_guard<std::mutex> lock(_vehicle_attitude.mutex);
            _vehicle_attitude.yaw_deg = -30.0f;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "DONE\n";

        return test_pitch_yaw("Look to the right", 0.0f, 90.0f, Mode::Lock) &&
            test_pitch_yaw("Look 60 degrees to the left", 0.0f, -90.0f, Mode::Lock) &&
            test_pitch_yaw("Tilt 45 degrees down", -45.0f, -90.0f, Mode::Lock) &&
            test_pitch_yaw("Look 60 degrees to the right", -45.0f, 90.0f, Mode::Lock) &&
            test_pitch_yaw("Look forward again", 0.0f, 0.0f, Mode::Lock);
    }

    bool test_pitch_yaw(const std::string& description, float pitch_deg, float yaw_deg, Mode mode)
    {
        std::cout << test_prefix << description << "... " << std::flush;
        {
            std::lock_guard<std::mutex> lock(_attitude_setpoint.mutex);
            _attitude_setpoint.pitch_deg = pitch_deg;
            _attitude_setpoint.yaw_deg = yaw_deg;
            _attitude_setpoint.mode = mode;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        const float margin_deg = 2.0f;

        if (mode == Mode::Follow) {
            std::lock_guard<std::mutex> lock(_vehicle_attitude.mutex);
            yaw_deg += _vehicle_attitude.yaw_deg;
        }

        std::lock_guard<std::mutex> lock(_gimbal_attitude.mutex);

        bool pitch_fail = false;
        bool yaw_fail = false;

        // TODO: We should do this check in quaternion to avoid gimbal locks.
        //       For now we avoid the check close to it.

        if (_gimbal_attitude.pitch_deg > pitch_deg + margin_deg) {
            pitch_fail = true;
        } else if (_gimbal_attitude.pitch_deg < pitch_deg - margin_deg) {
            pitch_fail = true;
        }

        if (_gimbal_attitude.pitch_deg < 80.0f && _gimbal_attitude.pitch_deg > -80.0) {
            if (_gimbal_attitude.yaw_deg > yaw_deg + margin_deg) {
                yaw_fail = true;
            } else if (_gimbal_attitude.yaw_deg < yaw_deg - margin_deg) {
                yaw_fail = true;
            }
        }

        if (pitch_fail || yaw_fail) {
            std::cout << "FAIL\n";
        } else {
            std::cout << "PASS\n";
        }

        if (pitch_fail) {
            std::cout << "-> pitch is " << _gimbal_attitude.pitch_deg << " deg instead of " << pitch_deg << " deg\n";
        }

        if (yaw_fail) {
            std::cout << "-> yaw is " << _gimbal_attitude.yaw_deg << " deg instead of " << yaw_deg << " deg\n";
        }

        return !(pitch_fail || yaw_fail);
    }


private:
    AttitudeSetpoint& _attitude_setpoint;
    VehicleAttitude& _vehicle_attitude;
    GimbalAttitude& _gimbal_attitude;
};

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

    GimbalAttitude gimbal_attitude {};
    subscribe_to_gimbal_device_attitude_status(mavlink_passthrough, gimbal_attitude);

    AttitudeSetpoint attitude_setpoint {};
    VehicleAttitude vehicle_attitude {};
    Sender sender(mavlink_passthrough, attitude_setpoint, vehicle_attitude);

    if (!wait_for_yaw_estimator_to_converge(gimbal_attitude)) {
        return 1;
    }

    Tester tester(attitude_setpoint, vehicle_attitude, gimbal_attitude);

    if (!tester.test_pitch()) {
        return 1;
    }

    if (!tester.test_yaw_follow()) {
        return 1;
    }

    if (!tester.test_yaw_lock()) {
        return 1;
    }

    return 0;
}

bool wait_for_yaw_estimator_to_converge(const GimbalAttitude& gimbal_attitude)
{
    std::cout << test_prefix << "Waiting for yaw estimator to converge..." << std::flush;
    for (unsigned i = 0; i < 200; ++i) {
        {
            std::lock_guard<std::mutex> lock(gimbal_attitude.mutex);
            if (gimbal_attitude.yaw_deg < 1.0f && gimbal_attitude.yaw_deg > -1.0f) {
                std::cout << "PASS" << std::endl;
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "FAIL\n";
    std::cout << "-> timeout waiting for yaw to converge to 0" << std::endl;

    return false;
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

bool request_gimbal_device_information(MavlinkPassthrough& mavlink_passthrough)
{
    MavlinkPassthrough::CommandLong command;
    command.command = MAV_CMD_REQUEST_MESSAGE;
    command.param1 = static_cast<float>(MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION);
    command.target_sysid = mavlink_passthrough.get_target_sysid();
    command.target_compid = mavlink_passthrough.get_target_compid();

    return (mavlink_passthrough.send_command_long(command) == MavlinkPassthrough::Result::Success);
}

void subscribe_to_gimbal_device_attitude_status(MavlinkPassthrough& mavlink_passthrough, GimbalAttitude& gimbal_attitude)
{
    mavlink_passthrough.subscribe_message_async(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS,
        [&gimbal_attitude](const mavlink_message_t& message) {
        mavlink_gimbal_device_attitude_status_t attitude_status;
        mavlink_msg_gimbal_device_attitude_status_decode(&message, &attitude_status);

        float roll_rad, pitch_rad, yaw_rad;
        mavlink_quaternion_to_euler(attitude_status.q, &roll_rad, &pitch_rad, &yaw_rad);

        std::lock_guard<std::mutex> lock(gimbal_attitude.mutex);
        gimbal_attitude.roll_deg = degrees(roll_rad);
        gimbal_attitude.pitch_deg = degrees(pitch_rad);
        gimbal_attitude.yaw_deg = degrees(yaw_rad);
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
