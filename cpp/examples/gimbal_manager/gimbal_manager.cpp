//
// Example implementing a MAVLink gimbal manager (gimbal protocol v2) with an
// internally simulated gimbal device.
//
// The component identifies as a MAVLink gimbal (MAV_COMP_ID_GIMBAL) with a
// built-in gimbal manager, so a ground station (e.g. QGroundControl or the
// MAVSDK gimbal example) can discover and control it:
// - It answers MAV_CMD_REQUEST_MESSAGE for GIMBAL_MANAGER_INFORMATION and
//   GIMBAL_DEVICE_INFORMATION.
// - It streams GIMBAL_MANAGER_STATUS (5 Hz) and GIMBAL_DEVICE_ATTITUDE_STATUS
//   (10 Hz).
// - It accepts setpoints via GIMBAL_MANAGER_SET_ATTITUDE,
//   GIMBAL_MANAGER_SET_PITCHYAW and MAV_CMD_DO_GIMBAL_MANAGER_PITCHYAW.
// - It tracks control ownership via MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE.
// - It points at a region of interest set via MAV_CMD_DO_SET_ROI_LOCATION
//   (and cancelled via MAV_CMD_DO_SET_ROI_NONE), using the vehicle's
//   GLOBAL_POSITION_INT and ATTITUDE to compute where to point.
//
// The gimbal device itself is simulated: pitch and yaw slew towards the
// setpoints with a limited angular rate.
//
// Run it connected to PX4 SITL:
// ./gimbal_manager udpin://0.0.0.0:14540
//

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/math_utils.hpp>
#include <mavsdk/plugins/mavlink_direct_server/mavlink_direct_server.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

using namespace mavsdk;
using json = nlohmann::json;

// MAVLink constants, so that we don't depend on the MAVLink C headers.
constexpr unsigned mav_cmd_do_set_roi_location = 195;
constexpr unsigned mav_cmd_do_set_roi_none = 197;
constexpr unsigned mav_cmd_request_message = 512;
constexpr unsigned mav_cmd_do_gimbal_manager_pitchyaw = 1000;
constexpr unsigned mav_cmd_do_gimbal_manager_configure = 1001;

constexpr unsigned message_id_gimbal_manager_information = 280;
constexpr unsigned message_id_gimbal_device_information = 283;

constexpr unsigned mav_result_accepted = 0;
constexpr unsigned mav_result_temporarily_rejected = 1;
constexpr unsigned mav_result_denied = 2;
constexpr unsigned mav_result_unsupported = 3;

constexpr unsigned mav_comp_id_autopilot1 = 1;

constexpr unsigned mav_frame_global = 0;
constexpr unsigned mav_frame_global_relative_alt = 3;
constexpr unsigned mav_frame_global_int = 5;
constexpr unsigned mav_frame_global_relative_alt_int = 6;

// GIMBAL_MANAGER_FLAGS and GIMBAL_DEVICE_FLAGS share the same bit values.
constexpr uint32_t gimbal_flags_neutral = 2;
constexpr uint32_t gimbal_flags_roll_lock = 4;
constexpr uint32_t gimbal_flags_pitch_lock = 8;
constexpr uint32_t gimbal_flags_yaw_lock = 16;
constexpr uint32_t gimbal_flags_yaw_in_vehicle_frame = 32;

// GIMBAL_MANAGER_CAP_FLAGS and GIMBAL_DEVICE_CAP_FLAGS share the same bit values.
constexpr uint32_t gimbal_cap_flags_has_neutral = 2;
constexpr uint32_t gimbal_cap_flags_has_pitch_axis = 32;
constexpr uint32_t gimbal_cap_flags_has_pitch_follow = 64;
constexpr uint32_t gimbal_cap_flags_has_pitch_lock = 128;
constexpr uint32_t gimbal_cap_flags_has_yaw_axis = 256;
constexpr uint32_t gimbal_cap_flags_has_yaw_follow = 512;
constexpr uint32_t gimbal_cap_flags_has_yaw_lock = 1024;

// This one only exists for GIMBAL_MANAGER_CAP_FLAGS, not for GIMBAL_DEVICE_CAP_FLAGS.
constexpr uint32_t gimbal_manager_cap_flags_can_point_location_global = 131072;

// No pitch follow: the simulated gimbal always stabilizes pitch (pitch lock).
constexpr uint32_t gimbal_device_cap_flags =
    gimbal_cap_flags_has_neutral | gimbal_cap_flags_has_pitch_axis |
    gimbal_cap_flags_has_pitch_lock | gimbal_cap_flags_has_yaw_axis |
    gimbal_cap_flags_has_yaw_follow | gimbal_cap_flags_has_yaw_lock;

constexpr uint32_t gimbal_manager_cap_flags =
    gimbal_device_cap_flags | gimbal_manager_cap_flags_can_point_location_global;

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

struct GlobalPosition {
    double lat_deg;
    double lon_deg;
    float alt_m;
};

struct PointingAngles {
    float pitch_deg;
    float yaw_deg;
};

float wrap_180(float angle_deg)
{
    while (angle_deg > 180.0f) {
        angle_deg -= 360.0f;
    }
    while (angle_deg < -180.0f) {
        angle_deg += 360.0f;
    }
    return angle_deg;
}

// Pitch and yaw (in vehicle frame) to point from the vehicle position at the
// ROI location, using a flat-earth approximation which is plenty for gimbal
// pointing distances.
PointingAngles pointing_angles_to_roi(
    const GlobalPosition& vehicle, const GlobalPosition& roi, float vehicle_yaw_deg)
{
    constexpr double earth_radius_m = 6371000.0;

    const double d_north_m = to_rad_from_deg(roi.lat_deg - vehicle.lat_deg) * earth_radius_m;
    const double d_east_m = to_rad_from_deg(roi.lon_deg - vehicle.lon_deg) * earth_radius_m *
                            std::cos(to_rad_from_deg(vehicle.lat_deg));
    const double distance_m = std::hypot(d_north_m, d_east_m);

    const auto bearing_deg = static_cast<float>(to_deg_from_rad(std::atan2(d_east_m, d_north_m)));
    const auto pitch_deg = static_cast<float>(
        to_deg_from_rad(std::atan2(static_cast<double>(roi.alt_m - vehicle.alt_m), distance_m)));

    return {pitch_deg, wrap_180(bearing_deg - vehicle_yaw_deg)};
}

class GimbalManager {
public:
    GimbalManager(MavlinkDirectServer& server, uint8_t own_sysid, uint8_t own_compid) :
        _server(server),
        _own_sysid(own_sysid),
        _own_compid(own_compid),
        _boot_time(std::chrono::steady_clock::now())
    {
        _server.subscribe_message(
            "COMMAND_LONG", [this](const auto& message) { handle_command(message, false); });
        _server.subscribe_message(
            "COMMAND_INT", [this](const auto& message) { handle_command(message, true); });
        _server.subscribe_message("GIMBAL_MANAGER_SET_ATTITUDE", [this](const auto& message) {
            handle_set_attitude(message);
        });
        _server.subscribe_message("GIMBAL_MANAGER_SET_PITCHYAW", [this](const auto& message) {
            handle_set_pitchyaw(message);
        });
        // Vehicle state, needed to compute where to point for a ROI.
        _server.subscribe_message("GLOBAL_POSITION_INT", [this](const auto& message) {
            handle_global_position_int(message);
        });
        _server.subscribe_message(
            "ATTITUDE", [this](const auto& message) { handle_attitude(message); });
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
            if (tick % 10 == 0) {
                send_gimbal_manager_status(); // 5 Hz
            }
            ++tick;
        }
    }

private:
    bool for_us(const json& fields, unsigned accepted_gimbal_device_id = 0) const
    {
        const unsigned target_system = fields.value("target_system", 0u);
        const unsigned target_component = fields.value("target_component", 0u);

        if (target_system != 0 && target_system != _own_sysid) {
            return false;
        }
        if (target_component != 0 && target_component != _own_compid) {
            return false;
        }
        // A gimbal_device_id of 0 addresses all gimbals of this manager.
        if (accepted_gimbal_device_id != 0 && accepted_gimbal_device_id != _own_compid) {
            return false;
        }
        return true;
    }

    bool in_control(uint8_t sysid, uint8_t compid)
    {
        // Be lenient while no one has taken control, to keep ad-hoc testing easy.
        if (_primary_sysid == 0 && _primary_compid == 0) {
            return true;
        }
        return sysid == _primary_sysid && compid == _primary_compid;
    }

    void handle_command(const MavlinkDirectServer::MavlinkMessage& message, bool is_int)
    {
        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded() || !for_us(fields)) {
            return;
        }

        // COMMAND_INT carries param5/param6/param7 as x/y/z.
        const char* param5_key = is_int ? "x" : "param5";
        const char* param7_key = is_int ? "z" : "param7";

        const unsigned command = fields.value("command", 0u);
        const auto sender_sysid = static_cast<uint8_t>(message.system_id);
        const auto sender_compid = static_cast<uint8_t>(message.component_id);

        switch (command) {
            case mav_cmd_request_message:
                handle_request_message(fields, sender_sysid, sender_compid);
                break;
            case mav_cmd_do_gimbal_manager_pitchyaw:
                handle_do_pitchyaw(fields, param5_key, param7_key, sender_sysid, sender_compid);
                break;
            case mav_cmd_do_gimbal_manager_configure:
                handle_do_configure(fields, param7_key, sender_sysid, sender_compid);
                break;
            case mav_cmd_do_set_roi_location:
                handle_do_set_roi_location(fields, is_int, sender_sysid, sender_compid);
                break;
            case mav_cmd_do_set_roi_none:
                handle_do_set_roi_none(fields, sender_sysid, sender_compid);
                break;
            default:
                // Only refuse commands explicitly addressed to us, a broadcast
                // command might be handled by another component.
                if (fields.value("target_component", 0u) == _own_compid) {
                    send_ack(sender_sysid, sender_compid, command, mav_result_unsupported);
                }
                break;
        }
    }

    void handle_request_message(const json& fields, uint8_t sender_sysid, uint8_t sender_compid)
    {
        const auto requested_id =
            static_cast<unsigned>(optional_float(fields, "param1").value_or(0.0f));

        switch (requested_id) {
            case message_id_gimbal_manager_information:
                send_ack(sender_sysid, sender_compid, mav_cmd_request_message, mav_result_accepted);
                send_gimbal_manager_information();
                break;
            case message_id_gimbal_device_information:
                send_ack(sender_sysid, sender_compid, mav_cmd_request_message, mav_result_accepted);
                send_gimbal_device_information();
                break;
            default:
                if (fields.value("target_component", 0u) == _own_compid) {
                    send_ack(
                        sender_sysid,
                        sender_compid,
                        mav_cmd_request_message,
                        mav_result_unsupported);
                }
                break;
        }
    }

    void handle_do_pitchyaw(
        const json& fields,
        const char* param5_key,
        const char* param7_key,
        uint8_t sender_sysid,
        uint8_t sender_compid)
    {
        const auto gimbal_device_id =
            static_cast<unsigned>(optional_float(fields, param7_key).value_or(0.0f));

        std::lock_guard<std::mutex> lock(_mutex);

        if (!for_us(fields, gimbal_device_id) || !in_control(sender_sysid, sender_compid)) {
            send_ack(
                sender_sysid, sender_compid, mav_cmd_do_gimbal_manager_pitchyaw, mav_result_denied);
            return;
        }

        // Params are in degrees, deg/s.
        const auto pitch_deg = optional_float(fields, "param1");
        const auto yaw_deg = optional_float(fields, "param2");
        const auto pitch_rate_deg_s = optional_float(fields, "param3");
        const auto yaw_rate_deg_s = optional_float(fields, "param4");

        if (auto flags = optional_float(fields, param5_key)) {
            apply_flags(static_cast<uint32_t>(flags.value()));
        }

        _roi.reset();
        _pitch_setpoint_deg = pitch_deg;
        _yaw_setpoint_deg = yaw_deg;
        _pitch_rate_setpoint_deg_s = pitch_rate_deg_s;
        _yaw_rate_setpoint_deg_s = yaw_rate_deg_s;

        std::cout << "Pitch/yaw setpoint from " << int(sender_sysid) << '/' << int(sender_compid)
                  << ": pitch " << pitch_deg.value_or(NAN) << " deg, yaw " << yaw_deg.value_or(NAN)
                  << " deg\n";

        send_ack(
            sender_sysid, sender_compid, mav_cmd_do_gimbal_manager_pitchyaw, mav_result_accepted);
    }

    void handle_do_configure(
        const json& fields, const char* param7_key, uint8_t sender_sysid, uint8_t sender_compid)
    {
        const auto gimbal_device_id =
            static_cast<unsigned>(optional_float(fields, param7_key).value_or(0.0f));

        std::lock_guard<std::mutex> lock(_mutex);

        if (!for_us(fields, gimbal_device_id)) {
            send_ack(
                sender_sysid,
                sender_compid,
                mav_cmd_do_gimbal_manager_configure,
                mav_result_denied);
            return;
        }

        apply_configure_id(fields, "param1", sender_sysid, _primary_sysid);
        apply_configure_id(fields, "param2", sender_compid, _primary_compid);
        apply_configure_id(fields, "param3", sender_sysid, _secondary_sysid);
        apply_configure_id(fields, "param4", sender_compid, _secondary_compid);

        std::cout << "Control configured by " << int(sender_sysid) << '/' << int(sender_compid)
                  << ": primary " << int(_primary_sysid) << '/' << int(_primary_compid)
                  << ", secondary " << int(_secondary_sysid) << '/' << int(_secondary_compid)
                  << '\n';

        send_ack(
            sender_sysid, sender_compid, mav_cmd_do_gimbal_manager_configure, mav_result_accepted);
    }

    void handle_do_set_roi_location(
        const json& fields, bool is_int, uint8_t sender_sysid, uint8_t sender_compid)
    {
        const auto gimbal_device_id =
            static_cast<unsigned>(optional_float(fields, "param1").value_or(0.0f));

        std::lock_guard<std::mutex> lock(_mutex);

        if (!for_us(fields, gimbal_device_id) || !in_control(sender_sysid, sender_compid)) {
            send_ack(sender_sysid, sender_compid, mav_cmd_do_set_roi_location, mav_result_denied);
            return;
        }

        if (!_vehicle_position) {
            // Without the vehicle position we don't know where to point.
            send_ack(
                sender_sysid,
                sender_compid,
                mav_cmd_do_set_roi_location,
                mav_result_temporarily_rejected);
            return;
        }

        GlobalPosition roi{};
        if (is_int) {
            // COMMAND_INT carries latitude/longitude as degE7 integers in x/y.
            roi.lat_deg = static_cast<double>(fields.value("x", int64_t{0})) * 1e-7;
            roi.lon_deg = static_cast<double>(fields.value("y", int64_t{0})) * 1e-7;
            roi.alt_m = optional_float(fields, "z").value_or(0.0f);

            // Convert the altitude to AMSL according to the frame.
            const unsigned frame = fields.value("frame", 0u);
            switch (frame) {
                case mav_frame_global:
                case mav_frame_global_int:
                    // Already AMSL.
                    break;
                case mav_frame_global_relative_alt:
                case mav_frame_global_relative_alt_int:
                    // Relative to home, and home AMSL is the vehicle AMSL minus
                    // its altitude relative to home.
                    roi.alt_m += _vehicle_position->alt_m - _vehicle_relative_alt_m;
                    break;
                default:
                    std::cout << "Denying ROI location: frame " << frame << " not supported\n";
                    send_ack(
                        sender_sysid,
                        sender_compid,
                        mav_cmd_do_set_roi_location,
                        mav_result_denied);
                    return;
            }
        } else {
            // COMMAND_LONG has no frame field, the altitude is taken as AMSL.
            roi.lat_deg = optional_float(fields, "param5").value_or(0.0f);
            roi.lon_deg = optional_float(fields, "param6").value_or(0.0f);
            roi.alt_m = optional_float(fields, "param7").value_or(0.0f);
        }

        _roi = roi;
        _pitch_rate_setpoint_deg_s.reset();
        _yaw_rate_setpoint_deg_s.reset();

        std::cout << "ROI set by " << int(sender_sysid) << '/' << int(sender_compid) << ": lat "
                  << roi.lat_deg << " deg, lon " << roi.lon_deg << " deg, alt " << roi.alt_m
                  << " m AMSL\n";

        send_ack(sender_sysid, sender_compid, mav_cmd_do_set_roi_location, mav_result_accepted);
    }

    void handle_do_set_roi_none(const json& fields, uint8_t sender_sysid, uint8_t sender_compid)
    {
        const auto gimbal_device_id =
            static_cast<unsigned>(optional_float(fields, "param1").value_or(0.0f));

        std::lock_guard<std::mutex> lock(_mutex);

        if (!for_us(fields, gimbal_device_id) || !in_control(sender_sysid, sender_compid)) {
            send_ack(sender_sysid, sender_compid, mav_cmd_do_set_roi_none, mav_result_denied);
            return;
        }

        // Back to neutral, as the spec asks for when there is no previous
        // manual input to return to.
        _roi.reset();
        _pitch_setpoint_deg = 0.0f;
        _yaw_setpoint_deg = 0.0f;
        _pitch_rate_setpoint_deg_s.reset();
        _yaw_rate_setpoint_deg_s.reset();

        std::cout << "ROI cleared by " << int(sender_sysid) << '/' << int(sender_compid) << '\n';

        send_ack(sender_sysid, sender_compid, mav_cmd_do_set_roi_none, mav_result_accepted);
    }

    void handle_global_position_int(const MavlinkDirectServer::MavlinkMessage& message)
    {
        // Only listen to the autopilot of our own system.
        if (message.system_id != _own_sysid || message.component_id != mav_comp_id_autopilot1) {
            return;
        }

        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded()) {
            return;
        }

        std::lock_guard<std::mutex> lock(_mutex);
        _vehicle_position = GlobalPosition{
            static_cast<double>(fields.value("lat", int64_t{0})) * 1e-7,
            static_cast<double>(fields.value("lon", int64_t{0})) * 1e-7,
            static_cast<float>(fields.value("alt", int64_t{0})) * 1e-3f};
        _vehicle_relative_alt_m =
            static_cast<float>(fields.value("relative_alt", int64_t{0})) * 1e-3f;
    }

    void handle_attitude(const MavlinkDirectServer::MavlinkMessage& message)
    {
        if (message.system_id != _own_sysid || message.component_id != mav_comp_id_autopilot1) {
            return;
        }

        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded()) {
            return;
        }

        std::lock_guard<std::mutex> lock(_mutex);
        if (const auto yaw_rad = optional_float(fields, "yaw")) {
            _vehicle_yaw_deg = to_deg_from_rad(*yaw_rad);
        }
    }

    // Apply one sysid/compid param of DO_GIMBAL_MANAGER_CONFIGURE with its special values:
    // -1: leave unchanged, -2: set to the sender's id, -3: remove if currently the sender's id.
    static void
    apply_configure_id(const json& fields, const char* key, uint8_t sender_id, uint8_t& id)
    {
        const auto param = optional_float(fields, key);
        if (!param) {
            return;
        }
        const int value = std::lround(param.value());

        if (value == -1) {
            // Leave unchanged.
        } else if (value == -2) {
            id = sender_id;
        } else if (value == -3) {
            if (id == sender_id) {
                id = 0;
            }
        } else if (value >= 0 && value <= UINT8_MAX) {
            id = static_cast<uint8_t>(value);
        }
    }

    void handle_set_attitude(const MavlinkDirectServer::MavlinkMessage& message)
    {
        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded()) {
            return;
        }

        std::lock_guard<std::mutex> lock(_mutex);

        if (!for_us(fields, fields.value("gimbal_device_id", 0u)) ||
            !in_control(
                static_cast<uint8_t>(message.system_id),
                static_cast<uint8_t>(message.component_id))) {
            return;
        }

        apply_flags(fields.value("flags", 0u));
        _roi.reset();

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

    void handle_set_pitchyaw(const MavlinkDirectServer::MavlinkMessage& message)
    {
        const auto fields = json::parse(message.fields_json, nullptr, false);
        if (fields.is_discarded()) {
            return;
        }

        std::lock_guard<std::mutex> lock(_mutex);

        if (!for_us(fields, fields.value("gimbal_device_id", 0u)) ||
            !in_control(
                static_cast<uint8_t>(message.system_id),
                static_cast<uint8_t>(message.component_id))) {
            return;
        }

        apply_flags(fields.value("flags", 0u));
        _roi.reset();

        // Fields are in radians, rad/s.
        const auto pitch_rad = optional_float(fields, "pitch");
        const auto yaw_rad = optional_float(fields, "yaw");
        const auto pitch_rate_rad_s = optional_float(fields, "pitch_rate");
        const auto yaw_rate_rad_s = optional_float(fields, "yaw_rate");

        _pitch_setpoint_deg =
            pitch_rad ? std::optional<float>(to_deg_from_rad(*pitch_rad)) : std::nullopt;
        _yaw_setpoint_deg =
            yaw_rad ? std::optional<float>(to_deg_from_rad(*yaw_rad)) : std::nullopt;
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

        if (_roi && _vehicle_position) {
            const auto angles = pointing_angles_to_roi(
                _vehicle_position.value(), _roi.value(), _vehicle_yaw_deg.value_or(0.0f));
            _pitch_setpoint_deg = angles.pitch_deg;
            _yaw_setpoint_deg = angles.yaw_deg;
        }

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

    void send_gimbal_manager_information()
    {
        send(
            "GIMBAL_MANAGER_INFORMATION",
            {
                {"time_boot_ms", time_boot_ms()},
                {"cap_flags", gimbal_manager_cap_flags},
                {"gimbal_device_id", _own_compid},
                {"roll_min", 0.0f},
                {"roll_max", 0.0f},
                {"pitch_min", to_rad_from_deg(pitch_min_deg)},
                {"pitch_max", to_rad_from_deg(pitch_max_deg)},
                {"yaw_min", to_rad_from_deg(yaw_min_deg)},
                {"yaw_max", to_rad_from_deg(yaw_max_deg)},
            });
    }

    void send_gimbal_device_information()
    {
        send(
            "GIMBAL_DEVICE_INFORMATION",
            {
                {"time_boot_ms", time_boot_ms()},
                {"vendor_name", "MAVSDK"},
                {"model_name", "Gimbal Manager Example"},
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
                {"gimbal_device_id", _own_compid},
            });
    }

    void send_gimbal_manager_status()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        send(
            "GIMBAL_MANAGER_STATUS",
            {
                {"time_boot_ms", time_boot_ms()},
                {"flags", current_flags()},
                {"gimbal_device_id", _own_compid},
                {"primary_control_sysid", _primary_sysid},
                {"primary_control_compid", _primary_compid},
                {"secondary_control_sysid", _secondary_sysid},
                {"secondary_control_compid", _secondary_compid},
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
    std::optional<GlobalPosition> _roi{};
    std::optional<GlobalPosition> _vehicle_position{};
    float _vehicle_relative_alt_m{0.0f};
    std::optional<float> _vehicle_yaw_deg{};
    uint8_t _primary_sysid{0};
    uint8_t _primary_compid{0};
    uint8_t _secondary_sysid{0};
    uint8_t _secondary_compid{0};
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

    GimbalManager gimbal_manager{
        mavlink_direct_server,
        static_cast<uint8_t>(config.get_system_id()),
        server_component->component_id()};

    std::cout << "Gimbal manager running as " << int(config.get_system_id()) << '/'
              << int(server_component->component_id()) << '\n';

    gimbal_manager.run();

    return 0;
}
