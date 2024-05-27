#include "follow_me_impl.h"
#include "system.h"
#include "px4_custom_mode.h"
#include <cmath>

namespace mavsdk {

FollowMeImpl::FollowMeImpl(System& system) : PluginImplBase(system)
{
    // (Lat, Lon, Alt) => double, (vx, vy, vz) => float
    _last_location = _target_location = FollowMe::TargetLocation{};
    _system_impl->register_plugin(this);
}

FollowMeImpl::FollowMeImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    // (Lat, Lon, Alt) => double, (vx, vy, vz) => float
    _last_location = _target_location = FollowMe::TargetLocation{};
    _system_impl->register_plugin(this);
}

FollowMeImpl::~FollowMeImpl()
{
    _system_impl->remove_call_every(_target_location_cookie);
    _system_impl->unregister_plugin(this);
}

void FollowMeImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) { process_heartbeat(message); },
        static_cast<void*>(this));
}

void FollowMeImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers(this);
}

void FollowMeImpl::enable()
{
    _system_impl->get_param_float_async(
        "FLW_TGT_HT",
        [this](MavlinkParameterClient::Result result, float value) {
            if (result == MavlinkParameterClient::Result::Success) {
                _config.follow_height_m = value;
            }
        },
        this);
    _system_impl->get_param_float_async(
        "FLW_TGT_DST",
        [this](MavlinkParameterClient::Result result, float value) {
            if (result == MavlinkParameterClient::Result::Success) {
                _config.follow_distance_m = value;
            }
        },
        this);
    _system_impl->get_param_float_async(
        "FLW_TGT_FA",
        [this](MavlinkParameterClient::Result result, float value) {
            if (result == MavlinkParameterClient::Result::Success) {
                _config.follow_angle_deg = value;
            }
        },
        this);
    _system_impl->get_param_float_async(
        "FLW_TGT_RS",
        [this](MavlinkParameterClient::Result result, float value) {
            if (result == MavlinkParameterClient::Result::Success) {
                _config.responsiveness = value;
            }
        },
        this);
    _system_impl->get_param_int_async(
        "FLW_TGT_ALT_M",
        [this](MavlinkParameterClient::Result result, int value) {
            if (result == MavlinkParameterClient::Result::Success) {
                _config.altitude_mode = static_cast<FollowMe::Config::FollowAltitudeMode>(value);
            }
        },
        this);
    _system_impl->get_param_float_async(
        "FLW_TGT_MAX_VEL",
        [this](MavlinkParameterClient::Result result, float value) {
            if (result == MavlinkParameterClient::Result::Success) {
                _config.max_tangential_vel_m_s = value;
            }
        },
        this);
}

void FollowMeImpl::disable()
{
    stop_sending_target_location();
}

FollowMe::Config FollowMeImpl::get_config() const
{
    return _config;
}

FollowMe::Result FollowMeImpl::set_config(const FollowMe::Config& config)
{
    // Valdidate configuration
    if (!is_config_ok(config)) {
        LogErr() << debug_str << "set_config() failed. Last configuration is preserved.";
        return FollowMe::Result::SetConfigFailed;
    }

    auto height = config.follow_height_m;
    auto distance = config.follow_distance_m;
    auto responsiveness = config.responsiveness;
    auto altitude_mode = config.altitude_mode;
    auto max_tangential_vel_m_s = config.max_tangential_vel_m_s;

    LogDebug() << "Waiting for the system confirmation of the new configuration..";

    bool success = true;

    // Send configuration to Vehicle
    if (_config.follow_height_m != height) {
        if (_system_impl->set_param_float("FLW_TGT_HT", height) ==
            MavlinkParameterClient::Result::Success) {
            _config.follow_height_m = height;
        } else {
            success = false;
        }
    }

    if (_config.follow_distance_m != distance) {
        if (_system_impl->set_param_float("FLW_TGT_DST", distance) ==
            MavlinkParameterClient::Result::Success) {
            _config.follow_distance_m = distance;
        } else {
            success = false;
        }
    }

    if (_config.follow_angle_deg != config.follow_angle_deg) {
        if (_system_impl->set_param_float("FLW_TGT_FA", config.follow_angle_deg) ==
            MavlinkParameterClient::Result::Success) {
            _config.follow_angle_deg = config.follow_angle_deg;
        } else {
            success = false;
        }
    }

    if (_config.responsiveness != responsiveness) {
        if (_system_impl->set_param_float("FLW_TGT_RS", responsiveness) ==
            MavlinkParameterClient::Result::Success) {
            _config.responsiveness = responsiveness;
        } else {
            success = false;
        }
    }

    if (_config.altitude_mode != altitude_mode) {
        if (_system_impl->set_param_int("FLW_TGT_ALT_M", static_cast<int32_t>(altitude_mode)) ==
            MavlinkParameterClient::Result::Success) {
            _config.altitude_mode = altitude_mode;
        } else {
            success = false;
        }
    }

    if (_config.max_tangential_vel_m_s != max_tangential_vel_m_s) {
        if (_system_impl->set_param_float("FLW_TGT_MAX_VEL", max_tangential_vel_m_s) ==
            MavlinkParameterClient::Result::Success) {
            _config.max_tangential_vel_m_s = max_tangential_vel_m_s;
        } else {
            success = false;
        }
    }

    return (success ? FollowMe::Result::Success : FollowMe::Result::SetConfigFailed);
}

FollowMe::Result FollowMeImpl::set_target_location(const FollowMe::TargetLocation& location)
{
    bool schedule_now = false;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _target_location = location;
        // We're interested only in lat, long.
        _estimation_capabilities |= (1 << static_cast<int>(EstimationCapabilities::POS));

        if (_mode != Mode::ACTIVE) {
            return FollowMe::Result::NotActive;
        }
        // If set already, reschedule it.
        if (_target_location_cookie) {
            _system_impl->reset_call_every(_target_location_cookie);
            // We also need to send it right now.
            schedule_now = true;

        } else {
            // Register now for sending now and in the next cycle.
            _target_location_cookie =
                _system_impl->add_call_every([this]() { send_target_location(); }, SENDER_RATE);
        }
    }

    if (schedule_now) {
        send_target_location();
    }

    return FollowMe::Result::Success;
}

FollowMe::TargetLocation FollowMeImpl::get_last_location() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _last_location;
}

bool FollowMeImpl::is_active() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _mode == Mode::ACTIVE;
}

FollowMe::Result FollowMeImpl::start()
{
    FollowMe::Result result =
        to_follow_me_result(_system_impl->set_flight_mode(FlightMode::FollowMe));

    if (result == FollowMe::Result::Success) {
        // If location was set before, lets send it to vehicle
        std::lock_guard<std::mutex> lock(
            _mutex); // locking is not necessary here but lets do it for integrity
        if (is_target_location_set()) {
            _target_location_cookie =
                _system_impl->add_call_every([this]() { send_target_location(); }, SENDER_RATE);
        }
    }
    return result;
}

FollowMe::Result FollowMeImpl::stop()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_mode == Mode::ACTIVE) {
            stop_sending_target_location();
        }
    }
    return to_follow_me_result(_system_impl->set_flight_mode(FlightMode::Hold));
}

bool FollowMeImpl::is_config_ok(const FollowMe::Config& config) const
{
    auto config_ok = false;

    if (config.follow_height_m < CONFIG_MIN_HEIGHT_M) {
        LogErr() << debug_str << "Err: Min height must be at least " << CONFIG_MIN_HEIGHT_M
                 << " meters";
    } else if (config.follow_distance_m < CONFIG_MIN_FOLLOW_DIST_M) {
        LogErr() << debug_str << "Err: Min Follow distance must be at least "
                 << CONFIG_MIN_FOLLOW_DIST_M << " meters";
    } else if (
        config.responsiveness < CONFIG_MIN_RESPONSIVENESS ||
        config.responsiveness > CONFIG_MAX_RESPONSIVENESS) {
        LogErr() << debug_str << "Err: Responsiveness must be in range ("
                 << CONFIG_MIN_RESPONSIVENESS << " to " << CONFIG_MAX_RESPONSIVENESS << ")";
    } else if (
        config.follow_angle_deg < CONFIG_MIN_FOLLOW_ANGLE ||
        config.follow_angle_deg > CONFIG_MAX_FOLLOW_ANGLE) {
        LogErr() << debug_str << "Err: Follow Angle must be in range " << CONFIG_MIN_FOLLOW_ANGLE
                 << " to " << CONFIG_MAX_FOLLOW_ANGLE << " degrees!";
    } else { // Config is OK
        config_ok = true;
    }

    return config_ok;
}

FollowMe::Result FollowMeImpl::to_follow_me_result(MavlinkCommandSender::Result result) const
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return FollowMe::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return FollowMe::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return FollowMe::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return FollowMe::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            // Fallthrough
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return FollowMe::Result::CommandDenied;
        case MavlinkCommandSender::Result::Timeout:
            return FollowMe::Result::Timeout;
        default:
            return FollowMe::Result::Unknown;
    }
}

bool FollowMeImpl::is_target_location_set() const
{
    // If the target's latitude is NAN, we assume that location is not set.
    // We assume that mutex was acquired by the caller
    return std::isfinite(_target_location.latitude_deg);
}

void FollowMeImpl::send_target_location()
{
    // Don't send if we're not in FollowMe mode.
    if (!is_active()) {
        return;
    }

    SteadyTimePoint now = _time.steady_time();
    // needed by http://mavlink.org/messages/common#FOLLOW_TARGET
    uint64_t elapsed_msec =
        static_cast<uint64_t>(_time.elapsed_since_s(now) * 1000); // milliseconds

    std::lock_guard<std::mutex> lock(_mutex);
    //   LogDebug() << debug_str <<  "Lat: " << _target_location.latitude_deg << " Lon: " <<
    //   _target_location.longitude_deg <<
    //	" Alt: " << _target_location.absolute_altitude_m;
    const int32_t lat_int = int32_t(std::round(_target_location.latitude_deg * 1e7));
    const int32_t lon_int = int32_t(std::round(_target_location.longitude_deg * 1e7));
    const float alt = static_cast<float>(_target_location.absolute_altitude_m);

    const float pos_std_dev[] = {NAN, NAN, NAN};
    const float vel[] = {
        static_cast<float>(_target_location.velocity_x_m_s),
        static_cast<float>(_target_location.velocity_y_m_s),
        static_cast<float>(_target_location.velocity_z_m_s)};
    const float accel_unknown[] = {NAN, NAN, NAN};
    const float attitude_q_unknown[] = {1.f, NAN, NAN, NAN};
    const float rates_unknown[] = {NAN, NAN, NAN};
    uint64_t custom_state = 0;

    if (!_system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message{};
            mavlink_msg_follow_target_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                elapsed_msec,
                _estimation_capabilities,
                lat_int,
                lon_int,
                alt,
                vel,
                accel_unknown,
                attitude_q_unknown,
                rates_unknown,
                pos_std_dev,
                custom_state);
            return message;
        })) {
        LogErr() << debug_str << "send_target_location() failed..";
    } else {
        _last_location = _target_location;
    }
}

void FollowMeImpl::stop_sending_target_location()
{
    // We assume that mutex was acquired by the caller
    _system_impl->remove_call_every(_target_location_cookie);
    _mode = Mode::NOT_ACTIVE;
}

void FollowMeImpl::process_heartbeat(const mavlink_message_t& message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    bool follow_me_active = false; // tells whether we're in FollowMe mode right now
    if (heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {
        px4::px4_custom_mode px4_custom_mode;
        px4_custom_mode.data = heartbeat.custom_mode;

        if (px4_custom_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_AUTO &&
            px4_custom_mode.sub_mode == px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET) {
            follow_me_active = true; // we're in FollowMe mode
        }
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!follow_me_active && _mode == Mode::ACTIVE) {
            // We're NOT in FollowMe mode anymore.
            // Lets stop sending target location updates
            stop_sending_target_location();
        } else if (follow_me_active && _mode == Mode::NOT_ACTIVE) {
            // We're in FollowMe mode now
            _mode = Mode::ACTIVE;
            return;
        }
    }
}

} // namespace mavsdk
