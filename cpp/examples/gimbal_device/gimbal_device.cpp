//
// Example implementing a MAVLink gimbal device (gimbal protocol v2).
//
// Unlike the gimbal_manager example (a combined gimbal manager and device in a
// single component), this is a standalone gimbal *device*. It is driven by a
// separate gimbal manager, for example a PX4 autopilot configured to output the
// MAVLink gimbal protocol v2. Manager and device then live on different
// components, which is the "separate component" case of the gimbal protocol.
//
// The component identifies as a MAVLink gimbal (MAV_COMP_ID_GIMBAL):
// - It answers MAV_CMD_REQUEST_MESSAGE for GIMBAL_DEVICE_INFORMATION.
// - It streams GIMBAL_DEVICE_ATTITUDE_STATUS (10 Hz) with gimbal_device_id 0,
//   i.e. the device is identified by its own component id.
// - It accepts setpoints via GIMBAL_DEVICE_SET_ATTITUDE from the manager.
//
// The gimbal device itself is simulated: pitch and yaw slew towards the
// setpoints with a limited angular rate.
//
// Run it connected to PX4 SITL, with the autopilot's gimbal manager configured
// to output MAVLink gimbal protocol v2 (MNT_MODE_OUT):
// ./gimbal_device udpin://0.0.0.0:14540
//

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/math_utils.hpp>
#include <mavsdk/plugins/mavlink_direct_server/mavlink_direct_server.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

using namespace mavsdk;
using json = nlohmann::json;

// MAVLink constants, so that we don't depend on the MAVLink C headers.
constexpr unsigned mav_cmd_request_message = 512;

constexpr unsigned message_id_gimbal_device_information = 283;

constexpr unsigned mav_result_accepted = 0;
constexpr unsigned mav_result_unsupported = 3;

// GIMBAL_DEVICE_FLAGS.
constexpr uint32_t gimbal_flags_neutral = 2;
constexpr uint32_t gimbal_flags_roll_lock = 4;
constexpr uint32_t gimbal_flags_pitch_lock = 8;
constexpr uint32_t gimbal_flags_yaw_lock = 16;
constexpr uint32_t gimbal_flags_yaw_in_vehicle_frame = 32;

// GIMBAL_DEVICE_CAP_FLAGS.
constexpr uint32_t gimbal_cap_flags_has_neutral = 2;
constexpr uint32_t gimbal_cap_flags_has_pitch_axis = 32;
constexpr uint32_t gimbal_cap_flags_has_pitch_lock = 128;
constexpr uint32_t gimbal_cap_flags_has_yaw_axis = 256;
constexpr uint32_t gimbal_cap_flags_has_yaw_follow = 512;
constexpr uint32_t gimbal_cap_flags_has_yaw_lock = 1024;

// No pitch follow: the simulated gimbal always stabilizes pitch (pitch lock).
constexpr uint32_t gimbal_device_cap_flags =
    gimbal_cap_flags_has_neutral | gimbal_cap_flags_has_pitch_axis |
    gimbal_cap_flags_has_pitch_lock | gimbal_cap_flags_has_yaw_axis |
    gimbal_cap_flags_has_yaw_follow | gimbal_cap_flags_has_yaw_lock;

// Limits of the simulated gimbal.
constexpr float pitch_min_deg = -90.0f;
constexpr float pitch_max_deg = 30.0f;
constexpr float yaw_min_deg = -160.0f;
constexpr float yaw_max_deg = 160.0f;
constexpr float default_slew_rate_deg_s = 60.0f;

// Float fields set to NaN ("don't change" in the gimbal protocol) arrive as
// JSON null, so only return a value for actual numbers.
std::optional<float> optional_float(const json& fields, const char* key)
{
    if (fields.contains(key) && fields[key].is_number()) {
        return fields[key].get<float>();
    }
    return std::nullopt;
}

class GimbalDevice {
public:
    GimbalDevice(MavlinkDirectServer& server, uint8_t own_sysid, uint8_t own_compid) :
        _server(server),
        _own_sysid(own_sysid),
        _own_compid(own_compid),
        _boot_time(std::chrono::steady_clock::now())
    {
        _server.subscribe_message(
            "COMMAND_LONG", [this](const auto& message) { handle_command(message); });
        _server.subscribe_message(
            "COMMAND_INT", [this](const auto& message) { handle_command(message); });
        _server.subscribe_message("GIMBAL_DEVICE_SET_ATTITUDE", [this](const auto& message) {
            handle_set_attitude(message);
        });
    }

    void run()
    {
        const auto interval = std::chrono::milliseconds(20); // 50 Hz
        unsigned tick = 0;

        while (true) {
            std::this_thread::sleep_for(interval);

            update_simulation(std::chrono::duration<float>(interval).count());

            if (tick % 5 == 0) {
                send_gimbal_device_attitude_status(); // 10 Hz
            }
            ++tick;
        }
    }

private:
    bool for_us(const json& fields) const
    {
        const unsigned target_system = fields.value("target_system", 0u);
        const unsigned target_component = fields.value("target_component", 0u);

        if (target_system != 0 && target_system != _own_sysid) {
            return false;
        }
        if (target_component != 0 && target_component != _own_compid) {
            return false;
        }
        return true;
    }

    void handle_command(const MavlinkDirectServer::MavlinkMessage& message)
    {
        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded() || !for_us(fields)) {
            return;
        }

        const unsigned command = fields.value("command", 0u);
        const auto sender_sysid = static_cast<uint8_t>(message.system_id);
        const auto sender_compid = static_cast<uint8_t>(message.component_id);

        if (command == mav_cmd_request_message) {
            handle_request_message(fields, sender_sysid, sender_compid);
        } else if (fields.value("target_component", 0u) == _own_compid) {
            // Only refuse commands explicitly addressed to us, a broadcast
            // command might be handled by another component.
            send_ack(sender_sysid, sender_compid, command, mav_result_unsupported);
        }
    }

    void handle_request_message(const json& fields, uint8_t sender_sysid, uint8_t sender_compid)
    {
        const auto requested_id =
            static_cast<unsigned>(optional_float(fields, "param1").value_or(0.0f));

        if (requested_id == message_id_gimbal_device_information) {
            send_ack(sender_sysid, sender_compid, mav_cmd_request_message, mav_result_accepted);
            send_gimbal_device_information();
        } else if (fields.value("target_component", 0u) == _own_compid) {
            send_ack(sender_sysid, sender_compid, mav_cmd_request_message, mav_result_unsupported);
        }
    }

    void handle_set_attitude(const MavlinkDirectServer::MavlinkMessage& message)
    {
        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded() || !for_us(fields)) {
            return;
        }

        std::lock_guard<std::mutex> lock(_mutex);

        apply_flags(fields.value("flags", 0u));

        // A quaternion with NaN elements (arriving as null) means "only use rates".
        std::optional<EulerAngle> setpoint{};
        if (fields.contains("q") && fields["q"].is_array() && fields["q"].size() == 4 &&
            std::all_of(fields["q"].begin(), fields["q"].end(), [](const json& element) {
                return element.is_number();
            })) {
            setpoint = to_euler_angle_from_quaternion(Quaternion{
                fields["q"][0].get<float>(),
                fields["q"][1].get<float>(),
                fields["q"][2].get<float>(),
                fields["q"][3].get<float>()});
        }

        if (setpoint) {
            _pitch_setpoint_deg = setpoint->pitch_deg;
            _yaw_setpoint_deg = setpoint->yaw_deg;
        } else {
            _pitch_setpoint_deg.reset();
            _yaw_setpoint_deg.reset();
        }

        const auto pitch_rate_rad_s = optional_float(fields, "angular_velocity_y");
        const auto yaw_rate_rad_s = optional_float(fields, "angular_velocity_z");
        _pitch_rate_setpoint_deg_s = pitch_rate_rad_s ?
                                         std::optional<float>(to_deg_from_rad(*pitch_rate_rad_s)) :
                                         std::nullopt;
        _yaw_rate_setpoint_deg_s =
            yaw_rate_rad_s ? std::optional<float>(to_deg_from_rad(*yaw_rate_rad_s)) : std::nullopt;
    }

    // Needs to be called with _mutex held.
    void apply_flags(uint32_t flags)
    {
        _yaw_lock = (flags & gimbal_flags_yaw_lock) != 0;

        if (flags & gimbal_flags_neutral) {
            _pitch_setpoint_deg = 0.0f;
            _yaw_setpoint_deg = 0.0f;
        }
    }

    void update_simulation(float dt_s)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        update_axis(
            dt_s,
            _pitch_setpoint_deg,
            _pitch_rate_setpoint_deg_s,
            pitch_min_deg,
            pitch_max_deg,
            _pitch_deg,
            _pitch_rate_deg_s);
        update_axis(
            dt_s,
            _yaw_setpoint_deg,
            _yaw_rate_setpoint_deg_s,
            yaw_min_deg,
            yaw_max_deg,
            _yaw_deg,
            _yaw_rate_deg_s);
    }

    static void update_axis(
        float dt_s,
        const std::optional<float>& setpoint_deg,
        const std::optional<float>& rate_setpoint_deg_s,
        float min_deg,
        float max_deg,
        float& angle_deg,
        float& rate_deg_s)
    {
        const float previous_deg = angle_deg;

        if (setpoint_deg) {
            // Slew towards the angle setpoint, the rate setpoint (if any) caps
            // the transition rate.
            const float slew_rate_deg_s =
                std::fabs(rate_setpoint_deg_s.value_or(default_slew_rate_deg_s));
            const float max_step_deg = slew_rate_deg_s * dt_s;
            angle_deg += std::clamp(setpoint_deg.value() - angle_deg, -max_step_deg, max_step_deg);
        } else if (rate_setpoint_deg_s) {
            angle_deg += rate_setpoint_deg_s.value() * dt_s;
        }

        angle_deg = std::clamp(angle_deg, min_deg, max_deg);
        rate_deg_s = (angle_deg - previous_deg) / dt_s;
    }

    uint32_t current_flags() const
    {
        uint32_t flags =
            gimbal_flags_roll_lock | gimbal_flags_pitch_lock | gimbal_flags_yaw_in_vehicle_frame;
        if (_yaw_lock) {
            flags |= gimbal_flags_yaw_lock;
        }
        return flags;
    }

    uint32_t time_boot_ms() const
    {
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::steady_clock::now() - _boot_time)
                                         .count());
    }

    void send_ack(uint8_t target_sysid, uint8_t target_compid, unsigned command, unsigned result)
    {
        send(
            "COMMAND_ACK",
            {
                {"command", command},
                {"result", result},
                {"progress", 0},
                {"result_param2", 0},
                {"target_system", target_sysid},
                {"target_component", target_compid},
            });
    }

    void send_gimbal_device_information()
    {
        send(
            "GIMBAL_DEVICE_INFORMATION",
            {
                {"time_boot_ms", time_boot_ms()},
                {"vendor_name", "MAVSDK"},
                {"model_name", "Gimbal Device Example"},
                {"custom_name", ""},
                {"firmware_version", 0},
                {"hardware_version", 0},
                {"uid", 0},
                {"cap_flags", gimbal_device_cap_flags},
                {"custom_cap_flags", 0},
                {"roll_min", 0.0f},
                {"roll_max", 0.0f},
                {"pitch_min", to_rad_from_deg(pitch_min_deg)},
                {"pitch_max", to_rad_from_deg(pitch_max_deg)},
                {"yaw_min", to_rad_from_deg(yaw_min_deg)},
                {"yaw_max", to_rad_from_deg(yaw_max_deg)},
                // A separate gimbal device is identified by its own component id,
                // so the gimbal_device_id here stays 0.
                {"gimbal_device_id", 0},
            });
    }

    void send_gimbal_device_attitude_status()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        // Roll is always zero for this two-axis gimbal.
        const auto q = to_quaternion_from_euler_angle(EulerAngle{0.0f, _pitch_deg, _yaw_deg});

        send(
            "GIMBAL_DEVICE_ATTITUDE_STATUS",
            {
                {"target_system", 0},
                {"target_component", 0},
                {"time_boot_ms", time_boot_ms()},
                {"flags", current_flags()},
                {"q", {q.w, q.x, q.y, q.z}},
                {"angular_velocity_x", 0.0f},
                {"angular_velocity_y", to_rad_from_deg(_pitch_rate_deg_s)},
                {"angular_velocity_z", to_rad_from_deg(_yaw_rate_deg_s)},
                {"failure_flags", 0},
                // With gimbal_device_id 0, the sending component id identifies the
                // gimbal device.
                {"gimbal_device_id", 0},
            });
    }

    void send(const std::string& message_name, const json& fields)
    {
        MavlinkDirectServer::MavlinkMessage message{};
        message.message_name = message_name;
        message.fields_json = fields.dump();

        const auto result = _server.send_message(message);
        if (result != MavlinkDirectServer::Result::Success) {
            std::cerr << "Failed to send " << message_name << ": " << result << '\n';
        }
    }

    MavlinkDirectServer& _server;
    const uint8_t _own_sysid;
    const uint8_t _own_compid;
    const std::chrono::steady_clock::time_point _boot_time;

    std::mutex _mutex{};
    std::optional<float> _pitch_setpoint_deg{};
    std::optional<float> _yaw_setpoint_deg{};
    std::optional<float> _pitch_rate_setpoint_deg_s{};
    std::optional<float> _yaw_rate_setpoint_deg_s{};
    float _pitch_deg{0.0f};
    float _yaw_deg{0.0f};
    float _pitch_rate_deg_s{0.0f};
    float _yaw_rate_deg_s{0.0f};
    bool _yaw_lock{false};
};

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <connection_url>\n"
                  << "E.g. towards PX4 SITL: " << argv[0] << " udpin://0.0.0.0:14540\n";
        return 1;
    }

    Mavsdk::Configuration config{ComponentType::Gimbal};
    Mavsdk mavsdk{config};

    const ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto server_component = mavsdk.server_component();
    MavlinkDirectServer mavlink_direct_server{server_component};

    GimbalDevice gimbal_device{
        mavlink_direct_server,
        static_cast<uint8_t>(config.get_system_id()),
        server_component->component_id()};

    std::cout << "Gimbal device running as " << int(config.get_system_id()) << '/'
              << int(server_component->component_id()) << '\n';

    gimbal_device.run();

    return 0;
}
