#include "global_include.h"
#include "offboard_impl.h"
#include "dronecode_sdk_impl.h"
#include "px4_custom_mode.h"

namespace dronecode_sdk {

OffboardImpl::OffboardImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

OffboardImpl::~OffboardImpl()
{
    _parent->unregister_plugin(this);
}

void OffboardImpl::init()
{
    // We need the system state.
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&OffboardImpl::process_heartbeat, this, std::placeholders::_1),
        this);
}

void OffboardImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void OffboardImpl::enable() {}

void OffboardImpl::disable() {}

Offboard::Result OffboardImpl::start()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_mode == Mode::NOT_ACTIVE) {
            return Offboard::Result::NO_SETPOINT_SET;
        }
    }

    return offboard_result_from_command_result(
        _parent->set_flight_mode(SystemImpl::FlightMode::OFFBOARD));
}

Offboard::Result OffboardImpl::stop()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_mode != Mode::NOT_ACTIVE) {
            stop_sending_setpoints();
        }
    }

    return offboard_result_from_command_result(
        _parent->set_flight_mode(SystemImpl::FlightMode::HOLD));
}

void OffboardImpl::start_async(Offboard::result_callback_t callback)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);

        if (_mode == Mode::NOT_ACTIVE) {
            if (callback) {
                callback(Offboard::Result::NO_SETPOINT_SET);
            }
            return;
        }
    }

    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::OFFBOARD,
        std::bind(&OffboardImpl::receive_command_result, this, std::placeholders::_1, callback));
}

void OffboardImpl::stop_async(Offboard::result_callback_t callback)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_mode != Mode::NOT_ACTIVE) {
            stop_sending_setpoints();
        }
    }

    _parent->set_flight_mode_async(
        SystemImpl::FlightMode::HOLD,
        std::bind(&OffboardImpl::receive_command_result, this, std::placeholders::_1, callback));
}

bool OffboardImpl::is_active() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return (_mode != Mode::NOT_ACTIVE);
}

void OffboardImpl::receive_command_result(MAVLinkCommands::Result result,
                                          const Offboard::result_callback_t &callback)
{
    Offboard::Result offboard_result = offboard_result_from_command_result(result);
    if (callback) {
        callback(offboard_result);
    }
}

void OffboardImpl::set_translation_ned(TranslationNED translation_ned)
{
    _mutex.lock();
    _translation_ned = translation_ned;

    if (_mode != Mode::TRANSLATION_NED) {
        if (_call_every_cookie) {
            // If we're already sending other setpoints, stop that now.
            _parent->remove_call_every(_call_every_cookie);
            _call_every_cookie = nullptr;
        }
        // We automatically send NED setpoints from now on.
        _parent->add_call_every(
            [this]() { send_translation_ned(); }, SEND_INTERVAL_S, &_call_every_cookie);

        _mode = Mode::TRANSLATION_NED;
    } else {
        // We're already sending these kind of setpoints. Since the setpoint change, let's
        // reschedule the next call, so we don't send setpoints too often.
        _parent->reset_call_every(_call_every_cookie);
    }
    _mutex.unlock();

    // also send it right now to reduce latency
    send_translation_ned();
}

void OffboardImpl::set_position_ned_yaw(Offboard::PositionNEDYaw position_ned_yaw)
{
    TranslationNED translation_ned{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    translation_ned.north_m = position_ned_yaw.north_m;
    translation_ned.east_m = position_ned_yaw.east_m;
    translation_ned.down_m = position_ned_yaw.down_m;
    translation_ned.yaw_deg = position_ned_yaw.yaw_deg;

    set_translation_ned(translation_ned);
}

void OffboardImpl::set_velocity_altitude_ned_yaw(
    Offboard::VelocityAltitudeNEDYaw velocity_altitude_ned_yaw)
{
    TranslationNED translation_ned{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    translation_ned.down_m = velocity_altitude_ned_yaw.down_m;
    translation_ned.north_m_s = velocity_altitude_ned_yaw.north_m_s;
    translation_ned.east_m_s = velocity_altitude_ned_yaw.east_m_s;
    translation_ned.yaw_deg = velocity_altitude_ned_yaw.yaw_deg;

    set_translation_ned(translation_ned);
}

void OffboardImpl::set_position_climbrate_ned_yaw(
    Offboard::PositionClimbRateNEDYaw position_climbrate_ned_yaw)
{
    TranslationNED translation_ned{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    translation_ned.north_m = position_climbrate_ned_yaw.north_m;
    translation_ned.east_m = position_climbrate_ned_yaw.east_m;
    translation_ned.down_m_s = position_climbrate_ned_yaw.down_m_s;
    translation_ned.yaw_deg = position_climbrate_ned_yaw.yaw_deg;

    set_translation_ned(translation_ned);
}

void OffboardImpl::set_position_climbrate_ned_yawspeed(
    Offboard::PositionClimbRateNEDYawspeed position_climbrate_ned_yawspeed)
{
    TranslationNED translation_ned{NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
    translation_ned.north_m = position_climbrate_ned_yawspeed.north_m;
    translation_ned.east_m = position_climbrate_ned_yawspeed.east_m;
    translation_ned.down_m_s = position_climbrate_ned_yawspeed.down_m_s;
    translation_ned.yawspeed_deg_s = position_climbrate_ned_yawspeed.yawspeed_deg_s;

    set_translation_ned(translation_ned);
}

void OffboardImpl::set_velocity_ned(Offboard::VelocityNEDYaw velocity_ned_yaw)
{
    _mutex.lock();
    _velocity_ned_yaw = velocity_ned_yaw;

    if (_mode != Mode::VELOCITY_NED) {
        if (_call_every_cookie) {
            // If we're already sending other setpoints, stop that now.
            _parent->remove_call_every(_call_every_cookie);
            _call_every_cookie = nullptr;
        }
        // We automatically send NED setpoints from now on.
        _parent->add_call_every(
            [this]() { send_velocity_ned(); }, SEND_INTERVAL_S, &_call_every_cookie);

        _mode = Mode::VELOCITY_NED;
    } else {
        // We're already sending these kind of setpoints. Since the setpoint change, let's
        // reschedule the next call, so we don't send setpoints too often.
        _parent->reset_call_every(_call_every_cookie);
    }
    _mutex.unlock();

    // also send it right now to reduce latency
    send_velocity_ned();
}

void OffboardImpl::set_velocity_body(Offboard::VelocityBodyYawspeed velocity_body_yawspeed)
{
    _mutex.lock();
    _velocity_body_yawspeed = velocity_body_yawspeed;

    if (_mode != Mode::VELOCITY_BODY) {
        if (_call_every_cookie) {
            // If we're already sending other setpoints, stop that now.
            _parent->remove_call_every(_call_every_cookie);
            _call_every_cookie = nullptr;
        }
        // We automatically send body setpoints from now on.
        _parent->add_call_every(
            [this]() { send_velocity_body(); }, SEND_INTERVAL_S, &_call_every_cookie);

        _mode = Mode::VELOCITY_BODY;
    } else {
        // We're already sending these kind of setpoints. Since the setpoint change, let's
        // reschedule the next call, so we don't send setpoints too often.
        _parent->reset_call_every(_call_every_cookie);
    }
    _mutex.unlock();

    // also send it right now to reduce latency
    send_velocity_body();
}

void OffboardImpl::send_translation_ned()
{
    // by default ingore all setpoints
    uint16_t IGNORE_X = (1 << 0);
    uint16_t IGNORE_Y = (1 << 1);
    uint16_t IGNORE_Z = (1 << 2);
    uint16_t IGNORE_VX = (1 << 3);
    uint16_t IGNORE_VY = (1 << 4);
    uint16_t IGNORE_VZ = (1 << 5);
    uint16_t IGNORE_AX = (1 << 6);
    uint16_t IGNORE_AY = (1 << 7);
    uint16_t IGNORE_AZ = (1 << 8);
    // uint16_t IS_FORCE = (1 << 9);
    uint16_t IGNORE_YAW = (1 << 10);
    uint16_t IGNORE_YAW_RATE = (1 << 11);

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float vz = 0.0f;
    float yaw = 0.0f;
    float yawspeed = 0.0f;
    const float afx = 0.0f;
    const float afy = 0.0f;
    const float afz = 0.0f;

    _mutex.lock();
    if (std::isfinite(_translation_ned.yaw_deg)) {
        yaw = to_rad_from_deg(_translation_ned.yaw_deg);
        // enable yaw
        IGNORE_YAW &= ~(1 << 10);
    }

    if (std::isfinite(_translation_ned.yawspeed_deg_s)) {
        yawspeed = to_rad_from_deg(_translation_ned.yawspeed_deg_s);
        // enable yawrate
        IGNORE_YAW_RATE &= ~(1 << 11);
    }

    if (std::isfinite(_translation_ned.north_m) && std::isfinite(_translation_ned.east_m)) {
        x = _translation_ned.north_m;
        y = _translation_ned.east_m;
        // enable x/y
        IGNORE_X &= ~(1 << 0);
        IGNORE_Y &= ~(1 << 1);
    }

    if (std::isfinite(_translation_ned.north_m_s) && std::isfinite(_translation_ned.east_m_s)) {
        vx = _translation_ned.north_m_s;
        vy = _translation_ned.east_m_s;
        // enable vx/vy
        IGNORE_VX &= ~(1 << 3);
        IGNORE_VY &= ~(1 << 4);
    }

    if (std::isfinite(_translation_ned.down_m)) {
        z = _translation_ned.down_m;
        // enable altitude
        IGNORE_Z &= ~(1 << 2);
    }

    if (std::isfinite(_translation_ned.down_m_s)) {
        vz = _translation_ned.down_m_s;
        // enable climbrate
        IGNORE_VZ &= ~(1 << 5);
    }

    _mutex.unlock();
    mavlink_message_t message;
    mavlink_msg_set_position_target_local_ned_pack(
        GCSClient::system_id,
        GCSClient::component_id,
        &message,
        static_cast<uint32_t>(_parent->get_time().elapsed_s() * 1e3),
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        MAV_FRAME_LOCAL_NED,
        IGNORE_X | IGNORE_Y | IGNORE_Z | IGNORE_VX | IGNORE_VY | IGNORE_VZ | IGNORE_AX | IGNORE_AY |
            IGNORE_AZ | IGNORE_YAW | IGNORE_YAW_RATE,
        x,
        y,
        z,
        vx,
        vy,
        vz,
        afx,
        afy,
        afz,
        yaw,
        yawspeed);
    _parent->send_message(message);
}

void OffboardImpl::send_velocity_ned()
{
    const static uint16_t IGNORE_X = (1 << 0);
    const static uint16_t IGNORE_Y = (1 << 1);
    const static uint16_t IGNORE_Z = (1 << 2);
    // const static uint16_t IGNORE_VX = (1 << 3);
    // const static uint16_t IGNORE_VY = (1 << 4);
    // const static uint16_t IGNORE_VZ = (1 << 5);
    const static uint16_t IGNORE_AX = (1 << 6);
    const static uint16_t IGNORE_AY = (1 << 7);
    const static uint16_t IGNORE_AZ = (1 << 8);
    // const static uint16_t IS_FORCE = (1 << 9);
    // const static uint16_t IGNORE_YAW = (1 << 10);
    const static uint16_t IGNORE_YAW_RATE = (1 << 11);

    _mutex.lock();
    const float yaw = to_rad_from_deg(_velocity_ned_yaw.yaw_deg);
    const float yaw_rate = 0.0f;
    const float x = 0.0f;
    const float y = 0.0f;
    const float z = 0.0f;
    const float vx = _velocity_ned_yaw.north_m_s;
    const float vy = _velocity_ned_yaw.east_m_s;
    const float vz = _velocity_ned_yaw.down_m_s;
    const float afx = 0.0f;
    const float afy = 0.0f;
    const float afz = 0.0f;
    _mutex.unlock();

    mavlink_message_t message;
    mavlink_msg_set_position_target_local_ned_pack(
        GCSClient::system_id,
        GCSClient::component_id,
        &message,
        static_cast<uint32_t>(_parent->get_time().elapsed_s() * 1e3),
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        MAV_FRAME_LOCAL_NED,
        IGNORE_X | IGNORE_Y | IGNORE_Z | IGNORE_AX | IGNORE_AY | IGNORE_AZ | IGNORE_YAW_RATE,
        x,
        y,
        z,
        vx,
        vy,
        vz,
        afx,
        afy,
        afz,
        yaw,
        yaw_rate);
    _parent->send_message(message);
}

void OffboardImpl::send_velocity_body()
{
    const static uint16_t IGNORE_X = (1 << 0);
    const static uint16_t IGNORE_Y = (1 << 1);
    const static uint16_t IGNORE_Z = (1 << 2);
    // const static uint16_t IGNORE_VX = (1 << 3);
    // const static uint16_t IGNORE_VY = (1 << 4);
    // const static uint16_t IGNORE_VZ = (1 << 5);
    const static uint16_t IGNORE_AX = (1 << 6);
    const static uint16_t IGNORE_AY = (1 << 7);
    const static uint16_t IGNORE_AZ = (1 << 8);
    // const static uint16_t IS_FORCE = (1 << 9);
    const static uint16_t IGNORE_YAW = (1 << 10);
    // const static uint16_t IGNORE_YAW_RATE = (1 << 11);

    _mutex.lock();
    const float yaw = 0.0f;
    const float yaw_rate = to_rad_from_deg(_velocity_body_yawspeed.yawspeed_deg_s);
    const float x = 0.0f;
    const float y = 0.0f;
    const float z = 0.0f;
    const float vx = _velocity_body_yawspeed.forward_m_s;
    const float vy = _velocity_body_yawspeed.right_m_s;
    const float vz = _velocity_body_yawspeed.down_m_s;
    const float afx = 0.0f;
    const float afy = 0.0f;
    const float afz = 0.0f;
    _mutex.unlock();

    mavlink_message_t message;
    mavlink_msg_set_position_target_local_ned_pack(
        GCSClient::system_id,
        GCSClient::component_id,
        &message,
        static_cast<uint32_t>(_parent->get_time().elapsed_s() * 1e3),
        _parent->get_system_id(),
        _parent->get_autopilot_id(),
        MAV_FRAME_BODY_NED,
        IGNORE_X | IGNORE_Y | IGNORE_Z | IGNORE_AX | IGNORE_AY | IGNORE_AZ | IGNORE_YAW,
        x,
        y,
        z,
        vx,
        vy,
        vz,
        afx,
        afy,
        afz,
        yaw,
        yaw_rate);
    _parent->send_message(message);
}

void OffboardImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    bool offboard_mode_active = false;
    if (heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {
        px4::px4_custom_mode px4_custom_mode;
        px4_custom_mode.data = heartbeat.custom_mode;

        if (px4_custom_mode.main_mode == px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD) {
            offboard_mode_active = true;
        }
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!offboard_mode_active && _mode != Mode::NOT_ACTIVE) {
            // It seems that we are no longer in offboard mode but still trying to send
            // setpoints. Let's stop for now.
            stop_sending_setpoints();
        }
    }
}

void OffboardImpl::stop_sending_setpoints()
{
    // We assume that we already acquired the mutex in this function.

    if (_call_every_cookie != nullptr) {
        _parent->remove_call_every(_call_every_cookie);
        _call_every_cookie = nullptr;
    }
    _mode = Mode::NOT_ACTIVE;
}

Offboard::Result OffboardImpl::offboard_result_from_command_result(MAVLinkCommands::Result result)
{
    switch (result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Offboard::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return Offboard::Result::NO_SYSTEM;
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return Offboard::Result::CONNECTION_ERROR;
        case MAVLinkCommands::Result::BUSY:
            return Offboard::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Offboard::Result::COMMAND_DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
            return Offboard::Result::TIMEOUT;
        default:
            return Offboard::Result::UNKNOWN;
    }
}

} // namespace dronecode_sdk
