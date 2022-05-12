#include "follow_me_impl.h"
#include "system.h"
#include "px4_custom_mode.h"
#include <cmath>

namespace mavsdk {

FollowMeImpl::FollowMeImpl(System& system) : PluginImplBase(system)
{
    // (Lat, Lon, Alt) => double, (vx, vy, vz) => float
    _last_location = _target_location = FollowMe::TargetLocation{};
    _parent->register_plugin(this);
}

FollowMeImpl::FollowMeImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    // (Lat, Lon, Alt) => double, (vx, vy, vz) => float
    _last_location = _target_location = FollowMe::TargetLocation{};
    _parent->register_plugin(this);
}

FollowMeImpl::~FollowMeImpl()
{
    if (_target_location_cookie) {
        _parent->remove_call_every(_target_location_cookie);
    }
    _parent->unregister_plugin(this);
}

void FollowMeImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) { process_heartbeat(message); },
        static_cast<void*>(this));
}

void FollowMeImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void FollowMeImpl::enable()
{
    _parent->get_param_float_async(
        "NAV_MIN_FT_HT",
        [this](MAVLinkParameters::Result result, float value) {
            if (result == MAVLinkParameters::Result::Success) {
                _config.min_height_m = value;
            }
        },
        this);
    _parent->get_param_float_async(
        "NAV_FT_DST",
        [this](MAVLinkParameters::Result result, float value) {
            if (result == MAVLinkParameters::Result::Success) {
                _config.follow_distance_m = value;
            }
        },
        this);
    _parent->get_param_int_async(
        "NAV_FT_FS",
        [this](MAVLinkParameters::Result result, int32_t value) {
            if (result == MAVLinkParameters::Result::Success) {
                _config.follow_direction = static_cast<FollowMe::Config::FollowDirection>(value);
            }
        },
        this);
    _parent->get_param_float_async(
        "NAV_FT_RS",
        [this](MAVLinkParameters::Result result, float value) {
            if (result == MAVLinkParameters::Result::Success) {
                _config.responsiveness = value;
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

    auto height = config.min_height_m;
    auto distance = config.follow_distance_m;
    int32_t direction = static_cast<int32_t>(config.follow_direction);
    auto responsiveness = config.responsiveness;

    LogDebug() << "Waiting for the system confirmation of the new configuration..";

    bool success = true;

    // Send configuration to Vehicle
    if (_config.min_height_m != height) {
        if (_parent->set_param_float("NAV_MIN_FT_HT", height) ==
            MAVLinkParameters::Result::Success) {
            _config.min_height_m = height;
        } else {
            success = false;
        }
    }
    if (_config.follow_distance_m != distance) {
        if (_parent->set_param_float("NAV_FT_DST", distance) ==
            MAVLinkParameters::Result::Success) {
            _config.follow_distance_m = distance;
        } else {
            success = false;
        }
    }
    if (_config.follow_direction != config.follow_direction) {
        if (_parent->set_param_int("NAV_FT_FS", direction) == MAVLinkParameters::Result::Success) {
            _config.follow_direction = static_cast<FollowMe::Config::FollowDirection>(direction);

        } else {
            success = false;
        }
    }
    if (_config.responsiveness != responsiveness) {
        if (_parent->set_param_float("NAV_FT_RS", responsiveness) ==
            MAVLinkParameters::Result::Success) {
            _config.responsiveness = responsiveness;
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
            _parent->reset_call_every(_target_location_cookie);
            // We also need to send it right now.
            schedule_now = true;

        } else {
            // Register now for sending now and in the next cycle.
            _parent->add_call_every(
                [this]() { send_target_location(); }, SENDER_RATE, &_target_location_cookie);
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
    FollowMe::Result result = to_follow_me_result(_parent->set_flight_mode(FlightMode::FollowMe));

    if (result == FollowMe::Result::Success) {
        // If location was set before, lets send it to vehicle
        std::lock_guard<std::mutex> lock(
            _mutex); // locking is not necessary here but lets do it for integrity
        if (is_target_location_set()) {
            _parent->add_call_every(
                [this]() { send_target_location(); }, SENDER_RATE, &_target_location_cookie);
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
    return to_follow_me_result(_parent->set_flight_mode(FlightMode::Hold));
}

bool FollowMeImpl::is_config_ok(const FollowMe::Config& config) const
{
    auto config_ok = false;

    if (config.min_height_m < CONFIG_MIN_HEIGHT_M) {
        LogErr() << debug_str << "Err: Min height must be atleast 8.0 meters";
    } else if (config.follow_distance_m < CONFIG_MIN_FOLLOW_DIST_M) {
        LogErr() << debug_str << "Err: Min Follow distance must be atleast 1.0 meter";
    } else if (
        config.follow_direction < FollowMe::Config::FollowDirection::None ||
        config.follow_direction > FollowMe::Config::FollowDirection::FrontLeft) {
        LogErr() << debug_str << "Err: Invalid Follow direction";
    } else if (
        config.responsiveness < CONFIG_MIN_RESPONSIVENESS ||
        config.responsiveness > CONFIG_MAX_RESPONSIVENESS) {
        LogErr() << debug_str << "Err: Responsiveness must be in range (0.0 to 1.0)";
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
        case MavlinkCommandSender::Result::CommandDenied:
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

    dl_time_t now = _time.steady_time();
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

    mavlink_message_t msg{};
    mavlink_msg_follow_target_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
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

    if (!_parent->send_message(msg)) {
        LogErr() << debug_str << "send_target_location() failed..";
    } else {
        _last_location = _target_location;
    }
}

void FollowMeImpl::stop_sending_target_location()
{
    // We assume that mutex was acquired by the caller
    if (_target_location_cookie) {
        _parent->remove_call_every(_target_location_cookie);
        _target_location_cookie = nullptr;
    }
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
