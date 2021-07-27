#include "telemetry_server_impl.h"

namespace mavsdk {

TelemetryServer::FlightMode
telemetry_flight_mode_from_flight_mode(SystemImpl::FlightMode flight_mode)
{
    switch (flight_mode) {
        case SystemImpl::FlightMode::Ready:
            return TelemetryServer::FlightMode::Ready;
        case SystemImpl::FlightMode::Takeoff:
            return TelemetryServer::FlightMode::Takeoff;
        case SystemImpl::FlightMode::Hold:
            return TelemetryServer::FlightMode::Hold;
        case SystemImpl::FlightMode::Mission:
            return TelemetryServer::FlightMode::Mission;
        case SystemImpl::FlightMode::ReturnToLaunch:
            return TelemetryServer::FlightMode::ReturnToLaunch;
        case SystemImpl::FlightMode::Land:
            return TelemetryServer::FlightMode::Land;
        case SystemImpl::FlightMode::Offboard:
            return TelemetryServer::FlightMode::Offboard;
        case SystemImpl::FlightMode::FollowMe:
            return TelemetryServer::FlightMode::FollowMe;
        case SystemImpl::FlightMode::Manual:
            return TelemetryServer::FlightMode::Manual;
        case SystemImpl::FlightMode::Posctl:
            return TelemetryServer::FlightMode::Posctl;
        case SystemImpl::FlightMode::Altctl:
            return TelemetryServer::FlightMode::Altctl;
        case SystemImpl::FlightMode::Rattitude:
            return TelemetryServer::FlightMode::Rattitude;
        case SystemImpl::FlightMode::Acro:
            return TelemetryServer::FlightMode::Acro;
        case SystemImpl::FlightMode::Stabilized:
            return TelemetryServer::FlightMode::Stabilized;
        default:
            return TelemetryServer::FlightMode::Unknown;
    }
}

TelemetryServerImpl::TelemetryServerImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
    _start_time = std::chrono::steady_clock::now();
}

TelemetryServerImpl::TelemetryServerImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

TelemetryServerImpl::~TelemetryServerImpl()
{
    _parent->unregister_plugin(this);
}

void TelemetryServerImpl::init()
{
    _parent->register_mavlink_command_handler(
        MAV_CMD_SET_MESSAGE_INTERVAL,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            // TO-DO handle mssage #245 and #33
            mavlink_message_t msg;
            mavlink_msg_command_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &msg,
                command.command,
                MAV_RESULT::MAV_RESULT_ACCEPTED,
                100,
                0,
                command.origin_system_id,
                command.origin_component_id);
            return msg;
        },
        this);

    _parent->register_mavlink_command_handler(
        MAV_CMD_COMPONENT_ARM_DISARM,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            TelemetryServer::ArmDisarm armDisarm{
                static_cast<int32_t>(command.params.param1),
                static_cast<int32_t>(command.params.param2)};

            // Check arm states - Ugly.
            uint8_t request_ack = MAV_RESULT_UNSUPPORTED;
            bool force = armDisarm.force == 21196;
            if (armDisarm.arm == 1) {
                request_ack = (_armable || (force && _force_armable)) ?
                                  MAV_RESULT::MAV_RESULT_ACCEPTED :
                                  MAV_RESULT_TEMPORARILY_REJECTED;
            } else {
                request_ack = (_disarmable || (force && _force_disarmable)) ?
                                  MAV_RESULT::MAV_RESULT_ACCEPTED :
                                  MAV_RESULT_TEMPORARILY_REJECTED;
            }

            if (request_ack == MAV_RESULT::MAV_RESULT_ACCEPTED) {
                _parent->set_server_armed(armDisarm.arm == 1);
            }

            auto result = (request_ack == MAV_RESULT::MAV_RESULT_ACCEPTED) ?
                              TelemetryServer::Result::Success :
                              TelemetryServer::Result::CommandDenied;

            _parent->call_user_callback(
                [this, armDisarm, result]() { _arm_disarm_callback(result, armDisarm); });

            mavlink_message_t msg;
            mavlink_msg_command_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &msg,
                command.command,
                request_ack,
                100,
                0,
                command.origin_system_id,
                command.origin_component_id);
            return msg;
        },
        this);

    _parent->register_mavlink_command_handler(
        MAV_CMD_DO_SET_MODE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            auto base_mode = static_cast<uint8_t>(command.params.param1);
            auto is_custom = (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) ==
                             MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
            TelemetryServer::FlightMode request_flight_mode = TelemetryServer::FlightMode::Unknown;

            auto custom_mode = static_cast<uint8_t>(command.params.param2);
            auto sub_custom_mode = static_cast<uint8_t>(command.params.param3);

            if (is_custom) {
                // PX4 uses custom modes
                px4_custom_mode px4_mode{};
                px4_mode.main_mode = custom_mode;
                px4_mode.sub_mode = sub_custom_mode;
                auto system_flight_mode = _parent->to_flight_mode_from_custom_mode(px4_mode.data);
                request_flight_mode = telemetry_flight_mode_from_flight_mode(system_flight_mode);
            } else {
                // TO DO non PX4 flight modes...
            }

            bool allow_mode = false;
            switch (request_flight_mode) {
                case TelemetryServer::FlightMode::Manual:
                    allow_mode = true;
                    break;
                case TelemetryServer::FlightMode::Mission:
                    allow_mode = _allowed_flight_modes.can_auto_mode;
                    break;
                case TelemetryServer::FlightMode::Stabilized:
                    allow_mode = _allowed_flight_modes.can_stabilize_mode;
                    break;
                default:
                    allow_mode = false;
                    break;
            }

            // PX4...
            px4_custom_mode px4_mode{};
            px4_mode.data = _parent->get_custom_mode();

            if (allow_mode) {
                px4_mode.main_mode = custom_mode;
                px4_mode.sub_mode = sub_custom_mode;
                _parent->set_custom_mode(px4_mode.data);
            }

            _parent->call_user_callback([this, allow_mode, request_flight_mode]() {
                _do_set_mode_callback(
                    allow_mode ? TelemetryServer::Result::Success :
                                 TelemetryServer::Result::CommandDenied,
                    request_flight_mode);
            });

            mavlink_message_t msg;
            mavlink_msg_command_ack_pack(
                _parent->get_own_system_id(),
                _parent->get_own_component_id(),
                &msg,
                command.command,
                allow_mode ? MAV_RESULT::MAV_RESULT_ACCEPTED : MAV_RESULT_TEMPORARILY_REJECTED,
                100,
                0,
                command.origin_system_id,
                command.origin_component_id);
            return msg;
        },
        this);
}

void TelemetryServerImpl::deinit() {}

void TelemetryServerImpl::enable() {}

void TelemetryServerImpl::disable() {}

TelemetryServer::Result TelemetryServerImpl::publish_position(
    TelemetryServer::Position position, TelemetryServer::VelocityNed velocity_ned)
{
    mavlink_message_t msg;
    mavlink_msg_global_position_int_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        get_boot_time(),
        static_cast<uint32_t>(position.latitude_deg) * 1E7,
        static_cast<uint32_t>(position.longitude_deg) * 1E7,
        static_cast<uint32_t>(position.absolute_altitude_m) * 1E3,
        static_cast<uint32_t>(position.relative_altitude_m) * 1E3,
        static_cast<uint32_t>(velocity_ned.north_m_s) * 1E2,
        static_cast<uint32_t>(velocity_ned.east_m_s) * 1E2,
        static_cast<uint32_t>(velocity_ned.down_m_s) * 1E2,
        0 // T0-DO: heading
    );

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_home(TelemetryServer::Position home)
{
    mavlink_message_t msg;
    float q[4] = {};
    mavlink_msg_home_position_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        home.latitude_deg * 1E7,
        home.longitude_deg * 1E7,
        static_cast<int>(home.absolute_altitude_m) * 1E-3,
        0, // Local X
        0, // Local Y
        0, // Local Z
        q, // surface normal transform
        NAN, // approach x
        NAN, // approach y
        NAN, // approach z
        get_boot_time() // TO-DO: System boot
    );

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_armed(bool is_armed)
{
    UNUSED(is_armed);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_raw_gps(
    TelemetryServer::RawGps raw_gps, TelemetryServer::GpsInfo gps_info)
{
    mavlink_message_t msg;
    mavlink_msg_gps_raw_int_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        raw_gps.timestamp_us,
        static_cast<uint8_t>(gps_info.fix_type),
        static_cast<uint32_t>(raw_gps.latitude_deg) * 1E7,
        static_cast<uint32_t>(raw_gps.longitude_deg) * 1E7,
        static_cast<uint32_t>(raw_gps.absolute_altitude_m) * 1E3,
        static_cast<uint32_t>(raw_gps.hdop) * 1E2,
        static_cast<uint32_t>(raw_gps.vdop) * 1E2,
        static_cast<uint32_t>(raw_gps.velocity_m_s) * 1E2,
        static_cast<uint32_t>(raw_gps.cog_deg) * 1E2,
        gps_info.num_satellites,
        static_cast<uint32_t>(raw_gps.altitude_ellipsoid_m) * 1E3,
        static_cast<uint32_t>(raw_gps.horizontal_uncertainty_m) * 1E3,
        static_cast<uint32_t>(raw_gps.vertical_uncertainty_m) * 1E3,
        static_cast<uint32_t>(raw_gps.velocity_uncertainty_m_s) * 1E3,
        static_cast<uint32_t>(raw_gps.heading_uncertainty_deg) * 1E5,
        static_cast<uint32_t>(raw_gps.yaw_deg) * 1E2);

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_battery(TelemetryServer::Battery battery)
{
    mavlink_message_t msg;

    uint16_t voltages[10] = {0};
    uint16_t voltages_ext[4] = {0};
    voltages[0] = static_cast<uint16_t>(battery.voltage_v) * 1E3;

    mavlink_msg_battery_status_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        0,
        MAV_BATTERY_FUNCTION_ALL,
        MAV_BATTERY_TYPE_LIPO,
        INT16_MAX,
        voltages,
        -1, // TODO publish all battery data
        -1,
        -1,
        static_cast<uint16_t>(battery.remaining_percent) * 1E2,
        0,
        MAV_BATTERY_CHARGE_STATE_UNDEFINED,
        voltages_ext,
        MAV_BATTERY_MODE_UNKNOWN,
        0);

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result
TelemetryServerImpl::publish_flight_mode(TelemetryServer::FlightMode flight_mode)
{
    UNUSED(flight_mode);

    // TODO :)
    return {};
}

TelemetryServer::Result
TelemetryServerImpl::publish_status_text(TelemetryServer::StatusText status_text)
{
    mavlink_message_t msg;

    int type = MAV_SEVERITY_INFO;
    switch (status_text.type) {
        case TelemetryServer::StatusTextType::Emergency:
            type = MAV_SEVERITY_EMERGENCY;
            break;
        case TelemetryServer::StatusTextType::Alert:
            type = MAV_SEVERITY_ALERT;
            break;
        case TelemetryServer::StatusTextType::Critical:
            type = MAV_SEVERITY_CRITICAL;
            break;
        case TelemetryServer::StatusTextType::Error:
            type = MAV_SEVERITY_ERROR;
            break;
        case TelemetryServer::StatusTextType::Warning:
            type = MAV_SEVERITY_WARNING;
            break;
        case TelemetryServer::StatusTextType::Notice:
            type = MAV_SEVERITY_NOTICE;
            break;
        case TelemetryServer::StatusTextType::Info:
            type = MAV_SEVERITY_INFO;
            break;
        case TelemetryServer::StatusTextType::Debug:
            type = MAV_SEVERITY_DEBUG;
            break;
        default:
            LogWarn() << "Unknown StatusText severity";
            type = MAV_SEVERITY_INFO;
            break;
    }

    mavlink_msg_statustext_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        type,
        status_text.text.data(),
        0,
        0);

    _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                 TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_odometry(TelemetryServer::Odometry odometry)
{
    UNUSED(odometry);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_position_velocity_ned(
    TelemetryServer::PositionVelocityNed position_velocity_ned)
{
    mavlink_message_t msg;
    mavlink_msg_local_position_ned_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        get_boot_time(),
        position_velocity_ned.position.north_m,
        position_velocity_ned.position.east_m,
        position_velocity_ned.position.down_m,
        position_velocity_ned.velocity.north_m_s,
        position_velocity_ned.velocity.east_m_s,
        position_velocity_ned.velocity.down_m_s);

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result
TelemetryServerImpl::publish_ground_truth(TelemetryServer::GroundTruth ground_truth)
{
    UNUSED(ground_truth);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_imu(TelemetryServer::Imu imu)
{
    UNUSED(imu);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_scaled_imu(TelemetryServer::Imu imu)
{
    UNUSED(imu);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_raw_imu(TelemetryServer::Imu imu)
{
    UNUSED(imu);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_health_all_ok(bool is_health_all_ok)
{
    UNUSED(is_health_all_ok);

    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::publish_unix_epoch_time(uint64_t time_us)
{
    UNUSED(time_us);

    // TODO :)
    return {};
}

void TelemetryServerImpl::subscribe_arm_disarm(TelemetryServer::ArmDisarmCallback callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _arm_disarm_callback = callback;
}

TelemetryServer::ArmDisarm TelemetryServerImpl::arm_disarm() const
{
    // TODO :)
    return {};
}

TelemetryServer::Result TelemetryServerImpl::set_armable(bool armable, bool force_armable)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _armable = armable;
    _force_armable = force_armable;
    return TelemetryServer::Result::Success;
}

TelemetryServer::Result TelemetryServerImpl::set_disarmable(bool disarmable, bool force_disarmable)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _disarmable = disarmable;
    _force_disarmable = force_disarmable;
    return TelemetryServer::Result::Success;
}

TelemetryServer::Result TelemetryServerImpl::publish_sys_status(
    TelemetryServer::Battery battery,
    bool rc_receiver_status,
    bool gyro_status,
    bool accel_status,
    bool mag_status,
    bool gps_status) const
{
    int32_t sensors = 0;

    if (rc_receiver_status)
        sensors |= MAV_SYS_STATUS_SENSOR_RC_RECEIVER;
    if (gyro_status)
        sensors |= MAV_SYS_STATUS_SENSOR_3D_GYRO;
    if (accel_status)
        sensors |= MAV_SYS_STATUS_SENSOR_3D_ACCEL;
    if (mag_status)
        sensors |= MAV_SYS_STATUS_SENSOR_3D_MAG;
    if (gps_status)
        sensors |= MAV_SYS_STATUS_SENSOR_GPS;

    mavlink_message_t msg;
    mavlink_msg_sys_status_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        sensors,
        sensors,
        sensors,
        0,
        static_cast<uint16_t>(battery.voltage_v) * 1E3,
        -1,
        static_cast<uint16_t>(battery.remaining_percent) * 1E2,
        0,
        0,
        0,
        0,
        0,
        0);

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

uint8_t to_mav_vtol_state(TelemetryServer::VTOLState vtol_state)
{
    switch (vtol_state) {
        case TelemetryServer::VTOLState::VtolUndefined:
            return MAV_VTOL_STATE_UNDEFINED;
        case TelemetryServer::VTOLState::VtolTransitionToFw:
            return MAV_VTOL_STATE_TRANSITION_TO_FW;
        case TelemetryServer::VTOLState::VtolTransitionToMc:
            return MAV_VTOL_STATE_TRANSITION_TO_MC;
        case TelemetryServer::VTOLState::VtolMc:
            return MAV_VTOL_STATE_MC;
        case TelemetryServer::VTOLState::VtolFw:
            return MAV_VTOL_STATE_FW;
        default:
            return MAV_VTOL_STATE_UNDEFINED;
    }
}

uint8_t to_mav_landed_state(TelemetryServer::LandedState landed_state)
{
    switch (landed_state) {
        case TelemetryServer::LandedState::InAir:
            return MAV_LANDED_STATE_IN_AIR;
        case TelemetryServer::LandedState::TakingOff:
            return MAV_LANDED_STATE_TAKEOFF;
        case TelemetryServer::LandedState::Landing:
            return MAV_LANDED_STATE_LANDING;
        case TelemetryServer::LandedState::OnGround:
            return MAV_LANDED_STATE_ON_GROUND;
        default:
            return MAV_LANDED_STATE_UNDEFINED;
    }
}

TelemetryServer::Result TelemetryServerImpl::publish_extended_sys_state(
    TelemetryServer::VTOLState vtol_state, TelemetryServer::LandedState landed_state) const
{
    mavlink_message_t msg;
    mavlink_msg_extended_sys_state_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        to_mav_vtol_state(vtol_state),
        to_mav_landed_state(landed_state));

    return _parent->send_message(msg) ? TelemetryServer::Result::Success :
                                        TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result
TelemetryServerImpl::set_allowable_flight_modes(TelemetryServer::AllowableFlightModes flight_modes)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _allowed_flight_modes = flight_modes;
}

TelemetryServer::AllowableFlightModes TelemetryServerImpl::get_allowable_flight_modes()
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    return _allowed_flight_modes;
}

void TelemetryServerImpl::subscribe_do_set_mode(TelemetryServer::DoSetModeCallback callback)
{
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _do_set_mode_callback = callback;
}

} // namespace mavsdk