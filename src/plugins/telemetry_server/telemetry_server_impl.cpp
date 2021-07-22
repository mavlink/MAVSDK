#include "telemetry_server_impl.h"

namespace mavsdk {

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
}

void TelemetryServerImpl::deinit() {}

void TelemetryServerImpl::enable() {}

void TelemetryServerImpl::disable() {}

void TelemetryServerImpl::publish_position_async(
    TelemetryServer::Position position,
    TelemetryServer::VelocityNed velocity_ned,
    const TelemetryServer::ResultCallback callback)
{
    UNUSED(position);

    UNUSED(velocity_ned);

    UNUSED(callback);
}

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

void TelemetryServerImpl::publish_home_async(
    TelemetryServer::Position home, const TelemetryServer::ResultCallback callback)
{
    UNUSED(home);

    UNUSED(callback);
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

void TelemetryServerImpl::publish_armed_async(
    bool is_armed, const TelemetryServer::ResultCallback callback)
{
    UNUSED(is_armed);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_armed(bool is_armed)
{
    UNUSED(is_armed);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_raw_gps_async(
    TelemetryServer::RawGps raw_gps,
    TelemetryServer::GpsInfo gps_info,
    const TelemetryServer::ResultCallback callback)
{
    UNUSED(raw_gps);

    UNUSED(gps_info);

    UNUSED(callback);
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

void TelemetryServerImpl::publish_battery_async(
    TelemetryServer::Battery battery, const TelemetryServer::ResultCallback callback)
{
    UNUSED(battery);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_battery(TelemetryServer::Battery battery)
{
    UNUSED(battery);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_flight_mode_async(
    TelemetryServer::FlightMode flight_mode, const TelemetryServer::ResultCallback callback)
{
    UNUSED(flight_mode);

    UNUSED(callback);
}

TelemetryServer::Result
TelemetryServerImpl::publish_flight_mode(TelemetryServer::FlightMode flight_mode)
{
    UNUSED(flight_mode);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_health_async(
    TelemetryServer::Health health, const TelemetryServer::ResultCallback callback)
{
    UNUSED(health);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_health(TelemetryServer::Health health)
{
    UNUSED(health);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_status_text_async(
    TelemetryServer::StatusText status_text, const TelemetryServer::ResultCallback callback)
{
    UNUSED(status_text);

    UNUSED(callback);
}

TelemetryServer::Result
TelemetryServerImpl::publish_status_text(TelemetryServer::StatusText status_text)
{
    UNUSED(status_text);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_odometry_async(
    TelemetryServer::Odometry odometry, const TelemetryServer::ResultCallback callback)
{
    UNUSED(odometry);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_odometry(TelemetryServer::Odometry odometry)
{
    UNUSED(odometry);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_position_velocity_ned_async(
    TelemetryServer::PositionVelocityNed position_velocity_ned,
    const TelemetryServer::ResultCallback callback)
{
    UNUSED(position_velocity_ned);

    UNUSED(callback);
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

void TelemetryServerImpl::publish_ground_truth_async(
    TelemetryServer::GroundTruth ground_truth, const TelemetryServer::ResultCallback callback)
{
    UNUSED(ground_truth);

    UNUSED(callback);
}

TelemetryServer::Result
TelemetryServerImpl::publish_ground_truth(TelemetryServer::GroundTruth ground_truth)
{
    UNUSED(ground_truth);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_imu_async(
    TelemetryServer::Imu imu, const TelemetryServer::ResultCallback callback)
{
    UNUSED(imu);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_imu(TelemetryServer::Imu imu)
{
    UNUSED(imu);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_scaled_imu_async(
    TelemetryServer::Imu imu, const TelemetryServer::ResultCallback callback)
{
    UNUSED(imu);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_scaled_imu(TelemetryServer::Imu imu)
{
    UNUSED(imu);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_raw_imu_async(
    TelemetryServer::Imu imu, const TelemetryServer::ResultCallback callback)
{
    UNUSED(imu);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_raw_imu(TelemetryServer::Imu imu)
{
    UNUSED(imu);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_health_all_ok_async(
    bool is_health_all_ok, const TelemetryServer::ResultCallback callback)
{
    UNUSED(is_health_all_ok);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_health_all_ok(bool is_health_all_ok)
{
    UNUSED(is_health_all_ok);

    // TODO :)
    return {};
}

void TelemetryServerImpl::publish_unix_epoch_time_async(
    uint64_t time_us, const TelemetryServer::ResultCallback callback)
{
    UNUSED(time_us);

    UNUSED(callback);
}

TelemetryServer::Result TelemetryServerImpl::publish_unix_epoch_time(uint64_t time_us)
{
    UNUSED(time_us);

    // TODO :)
    return {};
}

} // namespace mavsdk