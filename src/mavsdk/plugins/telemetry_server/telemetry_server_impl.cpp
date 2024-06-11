#include "telemetry_server_impl.h"
#include "unused.h"
#include <array>
#include <algorithm>

namespace mavsdk {

TelemetryServerImpl::TelemetryServerImpl(std::shared_ptr<ServerComponent> server_component) :
    ServerPluginImplBase(server_component)
{
    _server_component_impl->register_plugin(this);
    _start_time = std::chrono::steady_clock::now();
}

TelemetryServerImpl::~TelemetryServerImpl()
{
    _server_component_impl->unregister_plugin(this);
    std::lock_guard<std::mutex> lock(_mutex);
    for (const auto& request : _interval_requests) {
        _server_component_impl->remove_call_every(request.cookie);
    }
}

void TelemetryServerImpl::init()
{
    // Handle SET_MESSAGE_INTERVAL
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_SET_MESSAGE_INTERVAL,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            std::lock_guard<std::mutex> lock(_mutex);
            uint32_t msgid = static_cast<uint32_t>(command.params.param1);
            // Set interval to 1hz if 0 (default rate)
            uint32_t interval_ms =
                command.params.param2 == 0 ?
                    1000 :
                    static_cast<uint32_t>(static_cast<double>(command.params.param2) * 1E-3);
            LogDebug() << "Setting interval for msg id: " << std::to_string(msgid)
                       << " interval_ms:" << std::to_string(interval_ms);
            auto found = std::find_if(
                _interval_requests.begin(),
                _interval_requests.end(),
                [msgid](const RequestMsgInterval& item) { return item.msg_id == msgid; });

            if (found == _interval_requests.end() && command.params.param2 != -1) {
                // If not found interval already, add a new one
                _interval_requests.push_back({msgid, interval_ms});
                //_server_component_impl->add_call_every(
                //    [this, msgid]() {
                //        std::lock_guard<std::mutex> lock_interval(_mutex);
                //        if (_msg_cache.find(msgid) != _msg_cache.end()) {
                //            // Publish if callback exists :)
                //            _server_component_impl->send_message(_msg_cache.at(msgid));
                //        }
                //    },
                //    static_cast<double>(interval_ms) * 1E-3,
                //    &_interval_requests.back().cookie);
            } else {
                if (command.params.param2 == -1) {
                    // Deregister with -1 interval
                    _server_component_impl->remove_call_every(found->cookie);
                    _interval_requests.erase(found);
                } else {
                    // Update Interval
                    found->interval = interval_ms;
                    _server_component_impl->change_call_every(
                        static_cast<double>(interval_ms) * 1E-3, found->cookie);
                }
            }

            return _server_component_impl->make_command_ack_message(
                command, MAV_RESULT::MAV_RESULT_ACCEPTED);
        },
        this);

    // Handle REQUEST_MESSAGE
    _server_component_impl->register_mavlink_command_handler(
        MAV_CMD_REQUEST_MESSAGE,
        [this](const MavlinkCommandReceiver::CommandLong& command) {
            std::lock_guard<std::mutex> lock(_mutex);
            uint32_t msgid = static_cast<uint32_t>(command.params.param1);

            switch (msgid) {
                case MAVLINK_MSG_ID_HOME_POSITION:
                    if (_maybe_home) {
                        if (_send_home()) {
                            return _server_component_impl->make_command_ack_message(
                                command, MAV_RESULT::MAV_RESULT_ACCEPTED);
                        } else {
                            return _server_component_impl->make_command_ack_message(
                                command, MAV_RESULT::MAV_RESULT_FAILED);
                        }
                    } else {
                        return _server_component_impl->make_command_ack_message(
                            command, MAV_RESULT::MAV_RESULT_DENIED);
                    }

                default:
                    return _server_component_impl->make_command_ack_message(
                        command, MAV_RESULT::MAV_RESULT_DENIED);
            }
        },
        this);
}

void TelemetryServerImpl::deinit() {}

TelemetryServer::Result TelemetryServerImpl::publish_position(
    TelemetryServer::Position position,
    TelemetryServer::VelocityNed velocity_ned,
    TelemetryServer::Heading heading)
{
    return _server_component_impl->queue_message([&](MavlinkAddress mavlink_address,
                                                     uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_global_position_int_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            get_boot_time_ms(),
            static_cast<int32_t>(position.latitude_deg * 1E7),
            static_cast<int32_t>(position.longitude_deg * 1E7),
            static_cast<int32_t>(static_cast<double>(position.absolute_altitude_m) * 1E3),
            static_cast<int32_t>(static_cast<double>(position.relative_altitude_m) * 1E3),
            static_cast<int16_t>(static_cast<double>(velocity_ned.north_m_s) * 1E2),
            static_cast<int16_t>(static_cast<double>(velocity_ned.east_m_s) * 1E2),
            static_cast<int16_t>(static_cast<double>(velocity_ned.down_m_s) * 1E2),
            static_cast<uint16_t>(static_cast<double>(heading.heading_deg) * 1E2));
        return message;
    }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_home(TelemetryServer::Position home)
{
    _maybe_home = home;

    return _send_home() ? TelemetryServer::Result::Success :
                          TelemetryServer::Result::ConnectionError;
}

bool TelemetryServerImpl::_send_home()
{
    // Requires _maybe_home to be set.

    const auto home = _maybe_home.value();

    const float q[4] = {};

    return _server_component_impl->queue_message(
        [&](MavlinkAddress mavlink_address, uint8_t channel) {
            mavlink_message_t message;
            mavlink_msg_home_position_pack_chan(
                mavlink_address.system_id,
                mavlink_address.component_id,
                channel,
                &message,
                static_cast<int32_t>(home.latitude_deg * 1E7),
                static_cast<int32_t>(home.longitude_deg * 1E7),
                static_cast<int32_t>(static_cast<double>(home.absolute_altitude_m) * 1E-3),
                0, // Local X
                0, // Local Y
                0, // Local Z
                q, // surface normal transform
                NAN, // approach x
                NAN, // approach y
                NAN, // approach z
                get_boot_time_ms() // TO-DO: System boot
            );
            return message;
        });
}

TelemetryServer::Result TelemetryServerImpl::publish_raw_gps(
    TelemetryServer::RawGps raw_gps, TelemetryServer::GpsInfo gps_info)
{
    return _server_component_impl->queue_message([&](MavlinkAddress mavlink_address,
                                                     uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_gps_raw_int_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            raw_gps.timestamp_us,
            static_cast<uint8_t>(gps_info.fix_type),
            static_cast<int32_t>(raw_gps.latitude_deg * 1E7),
            static_cast<int32_t>(raw_gps.longitude_deg * 1E7),
            static_cast<int32_t>(static_cast<double>(raw_gps.absolute_altitude_m) * 1E3),
            static_cast<uint16_t>(static_cast<double>(raw_gps.hdop) * 1E2),
            static_cast<uint16_t>(static_cast<double>(raw_gps.vdop) * 1E2),
            static_cast<uint16_t>(static_cast<double>(raw_gps.velocity_m_s) * 1E2),
            static_cast<uint16_t>(static_cast<double>(raw_gps.cog_deg) * 1E2),
            static_cast<uint8_t>(gps_info.num_satellites),
            static_cast<int32_t>(static_cast<double>(raw_gps.altitude_ellipsoid_m) * 1E3),
            static_cast<uint32_t>(static_cast<double>(raw_gps.horizontal_uncertainty_m) * 1E3),
            static_cast<uint32_t>(static_cast<double>(raw_gps.vertical_uncertainty_m) * 1E3),
            static_cast<uint32_t>(static_cast<double>(raw_gps.velocity_uncertainty_m_s) * 1E3),
            static_cast<uint32_t>(static_cast<double>(raw_gps.heading_uncertainty_deg) * 1E5),
            static_cast<uint16_t>(static_cast<double>(raw_gps.yaw_deg) * 1E2));
        return message;
    }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_battery(TelemetryServer::Battery battery)
{
    uint16_t voltages[10] = {0};
    uint16_t voltages_ext[4] = {0};
    voltages[0] = static_cast<uint16_t>(static_cast<double>(battery.voltage_v) * 1E3);

    return _server_component_impl->queue_message(
               [&](MavlinkAddress mavlink_address, uint8_t channel) {
                   mavlink_message_t message;
                   mavlink_msg_battery_status_pack_chan(
                       mavlink_address.system_id,
                       mavlink_address.component_id,
                       channel,
                       &message,
                       0,
                       MAV_BATTERY_FUNCTION_ALL,
                       MAV_BATTERY_TYPE_LIPO,
                       INT16_MAX,
                       voltages,
                       -1, // TODO publish all battery data
                       -1,
                       -1,
                       static_cast<uint16_t>(static_cast<double>(battery.remaining_percent) * 1E2),
                       0,
                       MAV_BATTERY_CHARGE_STATE_UNDEFINED,
                       voltages_ext,
                       MAV_BATTERY_MODE_UNKNOWN,
                       0);
                   return message;
               }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result
TelemetryServerImpl::publish_distance_sensor(TelemetryServer::DistanceSensor distance_sensor)
{
    std::array<float, 4> q{0}; // Invalid Quaternion per mavlink spec

    return _server_component_impl->queue_message([&](MavlinkAddress mavlink_address,
                                                     uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_distance_sensor_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            0,
            static_cast<uint16_t>(static_cast<double>(distance_sensor.minimum_distance_m) * 1e2),
            static_cast<uint16_t>(static_cast<double>(distance_sensor.maximum_distance_m) * 1e2),
            static_cast<uint16_t>(static_cast<double>(distance_sensor.current_distance_m) * 1e2),
            MAV_DISTANCE_SENSOR_UNKNOWN,
            0,
            MAV_SENSOR_ROTATION_NONE,
            255,
            0,
            0,
            q.data(),
            0);
        return message;
    }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result
TelemetryServerImpl::publish_status_text(TelemetryServer::StatusText status_text)
{
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

    // Prevent memcpy in mavlink function to read outside of allocated data.
    status_text.text.resize(sizeof(mavlink_statustext_t::text));

    return _server_component_impl->queue_message(
               [&](MavlinkAddress mavlink_address, uint8_t channel) {
                   mavlink_message_t message;
                   mavlink_msg_statustext_pack_chan(
                       mavlink_address.system_id,
                       mavlink_address.component_id,
                       channel,
                       &message,
                       type,
                       status_text.text.data(),
                       0,
                       0);
                   return message;
               }) ?
               TelemetryServer::Result::Success :
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
    return _server_component_impl->queue_message(
               [&](MavlinkAddress mavlink_address, uint8_t channel) {
                   mavlink_message_t message;
                   mavlink_msg_local_position_ned_pack_chan(
                       mavlink_address.system_id,
                       mavlink_address.component_id,
                       channel,
                       &message,
                       get_boot_time_ms(),
                       position_velocity_ned.position.north_m,
                       position_velocity_ned.position.east_m,
                       position_velocity_ned.position.down_m,
                       position_velocity_ned.velocity.north_m_s,
                       position_velocity_ned.velocity.east_m_s,
                       position_velocity_ned.velocity.down_m_s);
                   return message;
               }) ?
               TelemetryServer::Result::Success :
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

TelemetryServer::Result TelemetryServerImpl::publish_unix_epoch_time(uint64_t time_us)
{
    return _server_component_impl->queue_message(
               [&](MavlinkAddress mavlink_address, uint8_t channel) {
                   mavlink_message_t message;
                   mavlink_msg_system_time_pack_chan(
                       mavlink_address.system_id,
                       mavlink_address.component_id,
                       channel,
                       &message,
                       time_us,
                       0 // TODO: add timestamping in general
                   );
                   return message;
               }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

TelemetryServer::Result TelemetryServerImpl::publish_sys_status(
    TelemetryServer::Battery battery,
    bool rc_receiver_status,
    bool gyro_status,
    bool accel_status,
    bool mag_status,
    bool gps_status)
{
    int32_t sensors = 0;

    if (rc_receiver_status) {
        sensors |= MAV_SYS_STATUS_SENSOR_RC_RECEIVER;
    }
    if (gyro_status) {
        sensors |= MAV_SYS_STATUS_SENSOR_3D_GYRO;
    }
    if (accel_status) {
        sensors |= MAV_SYS_STATUS_SENSOR_3D_ACCEL;
    }
    if (mag_status) {
        sensors |= MAV_SYS_STATUS_SENSOR_3D_MAG;
    }
    if (gps_status) {
        sensors |= MAV_SYS_STATUS_SENSOR_GPS;
    }

    return _server_component_impl->queue_message(
               [&](MavlinkAddress mavlink_address, uint8_t channel) {
                   mavlink_message_t message;
                   mavlink_msg_sys_status_pack_chan(
                       mavlink_address.system_id,
                       mavlink_address.component_id,
                       channel,
                       &message,
                       sensors,
                       sensors,
                       sensors,
                       0,
                       static_cast<uint16_t>(static_cast<double>(battery.voltage_v) * 1E3),
                       -1,
                       static_cast<uint16_t>(static_cast<double>(battery.remaining_percent) * 1E2),
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0);
                   return message;
               }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

uint8_t to_mav_vtol_state(TelemetryServer::VtolState vtol_state)
{
    switch (vtol_state) {
        case TelemetryServer::VtolState::Undefined:
            return MAV_VTOL_STATE_UNDEFINED;
        case TelemetryServer::VtolState::TransitionToFw:
            return MAV_VTOL_STATE_TRANSITION_TO_FW;
        case TelemetryServer::VtolState::TransitionToMc:
            return MAV_VTOL_STATE_TRANSITION_TO_MC;
        case TelemetryServer::VtolState::Mc:
            return MAV_VTOL_STATE_MC;
        case TelemetryServer::VtolState::Fw:
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
    TelemetryServer::VtolState vtol_state, TelemetryServer::LandedState landed_state)
{
    return _server_component_impl->queue_message(
               [&](MavlinkAddress mavlink_address, uint8_t channel) {
                   mavlink_message_t message;
                   mavlink_msg_extended_sys_state_pack_chan(
                       mavlink_address.system_id,
                       mavlink_address.component_id,
                       channel,
                       &message,
                       to_mav_vtol_state(vtol_state),
                       to_mav_landed_state(landed_state));
                   return message;
               }) ?
               TelemetryServer::Result::Success :
               TelemetryServer::Result::Unsupported;
}

} // namespace mavsdk
