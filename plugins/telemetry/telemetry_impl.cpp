#include "telemetry_impl.h"
#include "system.h"
#include "math_conversions.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <cmath>
#include <functional>

namespace dronecore {

TelemetryImpl::TelemetryImpl(System &system) :
    PluginImplBase(system),
    _position_mutex(),
    _position(Telemetry::Position {double(NAN), double(NAN), NAN, NAN}),
    _home_position_mutex(),
    _home_position(Telemetry::Position {double(NAN), double(NAN), NAN, NAN}),
    _in_air(false),
    _armed(false),
    _attitude_quaternion_mutex(),
    _attitude_quaternion(Telemetry::Quaternion {NAN, NAN, NAN, NAN}),
    _camera_attitude_euler_angle_mutex(),
    _camera_attitude_euler_angle(Telemetry::EulerAngle {NAN, NAN, NAN}),
    _ground_speed_ned_mutex(),
    _ground_speed_ned(Telemetry::GroundSpeedNED {NAN, NAN, NAN}),
    _gps_info_mutex(),
    _gps_info(Telemetry::GPSInfo {0, 0}),
    _battery_mutex(),
    _battery(Telemetry::Battery {NAN, NAN}),
    _flight_mode_mutex(),
    _flight_mode(Telemetry::FlightMode::UNKNOWN),
    _health_mutex(),
    _health(Telemetry::Health {false, false, false, false, false, false, false}),
    _rc_status_mutex(),
    _rc_status(Telemetry::RCStatus {false, false, 0.0f}),
    _position_subscription(nullptr),
    _home_position_subscription(nullptr),
    _in_air_subscription(nullptr),
    _armed_subscription(nullptr),
    _attitude_quaternion_subscription(nullptr),
    _attitude_euler_angle_subscription(nullptr),
    _camera_attitude_quaternion_subscription(nullptr),
    _camera_attitude_euler_angle_subscription(nullptr),
    _ground_speed_ned_subscription(nullptr),
    _gps_info_subscription(nullptr),
    _battery_subscription(nullptr),
    _flight_mode_subscription(nullptr),
    _health_subscription(nullptr),
    _health_all_ok_subscription(nullptr),
    _rc_status_subscription(nullptr),
    _ground_speed_ned_rate_hz(0.0),
    _position_rate_hz(-1.0)
{
    _parent->register_plugin(this);
}

TelemetryImpl::~TelemetryImpl()
{
    _parent->unregister_plugin(this);
}

void TelemetryImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        std::bind(&TelemetryImpl::process_global_position_int, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HOME_POSITION,
        std::bind(&TelemetryImpl::process_home_position, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ATTITUDE_QUATERNION,
        std::bind(&TelemetryImpl::process_attitude_quaternion, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MOUNT_ORIENTATION,
        std::bind(&TelemetryImpl::process_mount_orientation, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GPS_RAW_INT,
        std::bind(&TelemetryImpl::process_gps_raw_int, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        std::bind(&TelemetryImpl::process_extended_sys_state, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SYS_STATUS,
        std::bind(&TelemetryImpl::process_sys_status, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&TelemetryImpl::process_heartbeat, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_RC_CHANNELS,
        std::bind(&TelemetryImpl::process_rc_channels, this, _1), this);
}

void TelemetryImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void TelemetryImpl::enable()
{

    _parent->register_timeout_handler(
        std::bind(&TelemetryImpl::receive_rc_channels_timeout, this), 1.0, &_timeout_cookie);

    // FIXME: The calibration check should eventually be better than this.
    //        For now, we just do the same as QGC does.

    _parent->get_param_int_async(std::string("CAL_GYRO0_ID"),
                                 std::bind(&TelemetryImpl::receive_param_cal_gyro,
                                           this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));

    _parent->get_param_int_async(std::string("CAL_ACC0_ID"),
                                 std::bind(&TelemetryImpl::receive_param_cal_accel,
                                           this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));

    _parent->get_param_int_async(std::string("CAL_MAG0_ID"),
                                 std::bind(&TelemetryImpl::receive_param_cal_mag,
                                           this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));

#ifdef LEVEL_CALIBRATION
    _parent->get_param_float_async(std::string("SENS_BOARD_X_OFF"),
                                   std::bind(&TelemetryImpl::receive_param_cal_level,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
#else
    // If not available, just hardcode it to true.
    set_health_level_calibration(true);
#endif
}

void TelemetryImpl::disable()
{
    _parent->unregister_timeout_handler(_timeout_cookie);
}

Telemetry::Result TelemetryImpl::set_rate_position(double rate_hz)
{
    _position_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _ground_speed_ned_rate_hz);

    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_GLOBAL_POSITION_INT, max_rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_home_position(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_HOME_POSITION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_in_air(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_EXTENDED_SYS_STATE, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_attitude(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_ATTITUDE_QUATERNION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_camera_attitude(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_MOUNT_ORIENTATION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_ground_speed_ned(double rate_hz)
{
    _ground_speed_ned_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _ground_speed_ned_rate_hz);

    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_GLOBAL_POSITION_INT, max_rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_gps_info(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_GPS_RAW_INT, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_battery(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_SYS_STATUS, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_rc_status(double rate_hz)
{
    return telemetry_result_from_command_result(
               _parent->set_msg_rate(MAVLINK_MSG_ID_RC_CHANNELS, rate_hz));
}

void TelemetryImpl::set_rate_position_async(double rate_hz, Telemetry::result_callback_t callback)
{
    _position_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _ground_speed_ned_rate_hz);

    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        max_rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));

}

void TelemetryImpl::set_rate_home_position_async(double rate_hz,
                                                 Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_HOME_POSITION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_in_air_async(double rate_hz, Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_attitude_async(double rate_hz, Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_ATTITUDE_QUATERNION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_camera_attitude_async(double rate_hz,
                                                   Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_MOUNT_ORIENTATION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_ground_speed_ned_async(double rate_hz,
                                                    Telemetry::result_callback_t callback)
{
    _ground_speed_ned_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _ground_speed_ned_rate_hz);

    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        max_rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_gps_info_async(double rate_hz, Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_GPS_RAW_INT,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_battery_async(double rate_hz, Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_SYS_STATUS,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_rc_status_async(double rate_hz, Telemetry::result_callback_t callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_RC_CHANNELS,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

Telemetry::Result TelemetryImpl::telemetry_result_from_command_result(
    MAVLinkCommands::Result command_result)
{
    switch (command_result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Telemetry::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
            return Telemetry::Result::NO_SYSTEM;
        case MAVLinkCommands::Result::CONNECTION_ERROR:
            return Telemetry::Result::CONNECTION_ERROR;
        case MAVLinkCommands::Result::BUSY:
            return Telemetry::Result::BUSY;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Telemetry::Result::COMMAND_DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
            return Telemetry::Result::TIMEOUT;
        default:
            return Telemetry::Result::UNKNOWN;
    }
}

void TelemetryImpl::command_result_callback(MAVLinkCommands::Result command_result,
                                            const Telemetry::result_callback_t &callback)
{
    Telemetry::Result action_result = telemetry_result_from_command_result(command_result);

    callback(action_result);
}

void TelemetryImpl::process_global_position_int(const mavlink_message_t &message)
{
    mavlink_global_position_int_t global_position_int;
    mavlink_msg_global_position_int_decode(&message, &global_position_int);
    set_position(Telemetry::Position({global_position_int.lat * 1e-7,
                                      global_position_int.lon * 1e-7,
                                      global_position_int.alt * 1e-3f,
                                      global_position_int.relative_alt * 1e-3f
                                     }));
    set_ground_speed_ned({global_position_int.vx * 1e-2f,
                          global_position_int.vy * 1e-2f,
                          global_position_int.vz * 1e-2f
                         });

    if (_position_subscription) {
        _position_subscription(get_position());
    }

    if (_ground_speed_ned_subscription) {
        _ground_speed_ned_subscription(get_ground_speed_ned());
    }
}

void TelemetryImpl::process_home_position(const mavlink_message_t &message)
{
    mavlink_home_position_t home_position;
    mavlink_msg_home_position_decode(&message, &home_position);
    set_home_position(Telemetry::Position({home_position.latitude * 1e-7,
                                           home_position.longitude * 1e-7,
                                           home_position.altitude * 1e-3f,
                                           // the relative altitude of home is 0 by definition.
                                           0.0f
                                          }));

    set_health_home_position(true);

    if (_home_position_subscription) {
        _home_position_subscription(get_home_position());
    }
}

void TelemetryImpl::process_attitude_quaternion(const mavlink_message_t &message)
{
    mavlink_attitude_quaternion_t attitude_quaternion;
    mavlink_msg_attitude_quaternion_decode(&message, &attitude_quaternion);

    Telemetry::Quaternion quaternion {
        attitude_quaternion.q1,
        attitude_quaternion.q2,
        attitude_quaternion.q3,
        attitude_quaternion.q4
    };

    set_attitude_quaternion(quaternion);

    if (_attitude_quaternion_subscription) {
        _attitude_quaternion_subscription(get_attitude_quaternion());
    }

    if (_attitude_euler_angle_subscription) {
        _attitude_euler_angle_subscription(get_attitude_euler_angle());
    }
}

void TelemetryImpl::process_mount_orientation(const mavlink_message_t &message)
{
    mavlink_mount_orientation_t mount_orientation;
    mavlink_msg_mount_orientation_decode(&message, &mount_orientation);

    Telemetry::EulerAngle euler_angle {
        mount_orientation.roll,
        mount_orientation.pitch,
        mount_orientation.yaw_absolute
    };

    set_camera_attitude_euler_angle(euler_angle);

    if (_camera_attitude_quaternion_subscription) {
        _camera_attitude_quaternion_subscription(get_camera_attitude_quaternion());
    }

    if (_camera_attitude_euler_angle_subscription) {
        _camera_attitude_euler_angle_subscription(get_camera_attitude_euler_angle());
    }
}

void TelemetryImpl::process_gps_raw_int(const mavlink_message_t &message)
{
    mavlink_gps_raw_int_t gps_raw_int;
    mavlink_msg_gps_raw_int_decode(&message, &gps_raw_int);
    set_gps_info({gps_raw_int.satellites_visible,
                  gps_raw_int.fix_type
                 });

    // TODO: This is just an interim hack, we will have to look at
    //       estimator flags in order to decide if the position
    //       estimate is good enough.
    const bool gps_ok = ((gps_raw_int.fix_type >= 3) && (gps_raw_int.satellites_visible >= 8));

    set_health_global_position(gps_ok);
    // Local is not different from global for now until things like flow are in place.
    set_health_local_position(gps_ok);

    if (_gps_info_subscription) {
        _gps_info_subscription(get_gps_info());
    }
}

void TelemetryImpl::process_extended_sys_state(const mavlink_message_t &message)
{
    mavlink_extended_sys_state_t extended_sys_state;
    mavlink_msg_extended_sys_state_decode(&message, &extended_sys_state);

    if (extended_sys_state.landed_state == MAV_LANDED_STATE_IN_AIR) {
        set_in_air(true);
    } else if (extended_sys_state.landed_state == MAV_LANDED_STATE_ON_GROUND) {
        set_in_air(false);
    }
    // If landed_state is undefined, we use what we have received last.

    if (_in_air_subscription) {
        _in_air_subscription(in_air());
    }

}

void TelemetryImpl::process_sys_status(const mavlink_message_t &message)
{
    mavlink_sys_status_t sys_status;
    mavlink_msg_sys_status_decode(&message, &sys_status);
    set_battery(Telemetry::Battery({sys_status.voltage_battery * 1e-3f,
                                    // FIXME: it is strange calling it percent when the range goes from 0 to 1.
                                    sys_status.battery_remaining * 1e-2f
                                   }));

    if (_battery_subscription) {
        _battery_subscription(get_battery());
    }
}

void TelemetryImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    set_armed(((heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false));

    if (_armed_subscription) {
        _armed_subscription(armed());
    }

    if (heartbeat.base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {

        Telemetry::FlightMode flight_mode = to_flight_mode_from_custom_mode(heartbeat.custom_mode);
        set_flight_mode(flight_mode);

        if (_flight_mode_subscription) {
            _flight_mode_subscription(get_flight_mode());
        }
    }

    if (_health_subscription) {
        _health_subscription(get_health());
    }
    if (_health_all_ok_subscription) {
        _health_all_ok_subscription(get_health_all_ok());
    }
}

void TelemetryImpl::process_rc_channels(const mavlink_message_t &message)
{
    mavlink_rc_channels_t rc_channels;
    mavlink_msg_rc_channels_decode(&message, &rc_channels);

    bool rc_ok = (rc_channels.chancount > 0);
    set_rc_status(rc_ok, rc_channels.rssi);

    if (_rc_status_subscription) {
        _rc_status_subscription(get_rc_status());
    }

    _parent->refresh_timeout_handler(_timeout_cookie);
}

Telemetry::FlightMode TelemetryImpl::to_flight_mode_from_custom_mode(uint32_t custom_mode)
{
    px4::px4_custom_mode px4_custom_mode;
    px4_custom_mode.data = custom_mode;

    switch (px4_custom_mode.main_mode) {
        case px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD:
            return Telemetry::FlightMode::OFFBOARD;
        case px4::PX4_CUSTOM_MAIN_MODE_AUTO:
            switch (px4_custom_mode.sub_mode) {
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_READY:
                    return Telemetry::FlightMode::READY;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF:
                    return Telemetry::FlightMode::TAKEOFF;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER:
                    return Telemetry::FlightMode::HOLD;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION:
                    return Telemetry::FlightMode::MISSION;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL:
                    return Telemetry::FlightMode::RETURN_TO_LAUNCH;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND:
                    return Telemetry::FlightMode::LAND;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET:
                    return Telemetry::FlightMode::FOLLOW_ME;
                default:
                    return Telemetry::FlightMode::UNKNOWN;
            }
        default:
            return Telemetry::FlightMode::UNKNOWN;
    }
}

void TelemetryImpl::receive_param_cal_gyro(bool success, int value)
{
    if (!success) {
        LogErr() << "Error: Param for gyro cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_gyrometer_calibration(ok);
}

void TelemetryImpl::receive_param_cal_accel(bool success, int value)
{
    if (!success) {
        LogErr() << "Error: Param for accel cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_accelerometer_calibration(ok);
}

void TelemetryImpl::receive_param_cal_mag(bool success, int value)
{
    if (!success) {
        LogErr() << "Error: Param for mag cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_magnetometer_calibration(ok);
}

#ifdef LEVEL_CALIBRATION
void TelemetryImpl::receive_param_cal_level(bool success, float value)
{
    if (!success) {
        LogErr() << "Error: Param for level cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_level_calibration(ok);
}
#endif

void TelemetryImpl::receive_rc_channels_timeout()
{
    const bool rc_ok = false;
    set_rc_status(rc_ok, 0.0f);
}

Telemetry::Position TelemetryImpl::get_position() const
{
    std::lock_guard<std::mutex> lock(_position_mutex);
    return _position;
}

void TelemetryImpl::set_position(Telemetry::Position position)
{
    std::lock_guard<std::mutex> lock(_position_mutex);
    _position = position;
}

Telemetry::Position TelemetryImpl::get_home_position() const
{
    std::lock_guard<std::mutex> lock(_home_position_mutex);
    return _home_position;
}

void TelemetryImpl::set_home_position(Telemetry::Position home_position)
{
    std::lock_guard<std::mutex> lock(_home_position_mutex);
    _home_position = home_position;
}

bool TelemetryImpl::in_air() const
{
    return _in_air;
}

bool TelemetryImpl::armed() const
{
    return _armed;
}

void TelemetryImpl::set_in_air(bool in_air_new)
{
    _in_air = in_air_new;
}

void TelemetryImpl::set_armed(bool armed_new)
{
    _armed = armed_new;
}

Telemetry::Quaternion TelemetryImpl::get_attitude_quaternion() const
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    return _attitude_quaternion;
}

Telemetry::EulerAngle TelemetryImpl::get_attitude_euler_angle() const
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    Telemetry::EulerAngle euler = to_euler_angle_from_quaternion(_attitude_quaternion);

    return euler;
}

void TelemetryImpl::set_attitude_quaternion(Telemetry::Quaternion quaternion)
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    _attitude_quaternion = quaternion;
}

Telemetry::Quaternion TelemetryImpl::get_camera_attitude_quaternion() const
{
    std::lock_guard<std::mutex> lock(_camera_attitude_euler_angle_mutex);
    Telemetry::Quaternion quaternion
        = to_quaternion_from_euler_angle(_camera_attitude_euler_angle);

    return quaternion;
}

Telemetry::EulerAngle TelemetryImpl::get_camera_attitude_euler_angle() const
{
    std::lock_guard<std::mutex> lock(_camera_attitude_euler_angle_mutex);

    return _camera_attitude_euler_angle;
}

void TelemetryImpl::set_camera_attitude_euler_angle(Telemetry::EulerAngle euler_angle)
{
    std::lock_guard<std::mutex> lock(_camera_attitude_euler_angle_mutex);
    _camera_attitude_euler_angle = euler_angle;
}

Telemetry::GroundSpeedNED TelemetryImpl::get_ground_speed_ned() const
{
    std::lock_guard<std::mutex> lock(_ground_speed_ned_mutex);
    return _ground_speed_ned;
}

void TelemetryImpl::set_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned)
{
    std::lock_guard<std::mutex> lock(_ground_speed_ned_mutex);
    _ground_speed_ned = ground_speed_ned;
}

Telemetry::GPSInfo TelemetryImpl::get_gps_info() const
{
    std::lock_guard<std::mutex> lock(_gps_info_mutex);
    return _gps_info;
}

void TelemetryImpl::set_gps_info(Telemetry::GPSInfo gps_info)
{
    std::lock_guard<std::mutex> lock(_gps_info_mutex);
    _gps_info = gps_info;
}

Telemetry::Battery TelemetryImpl::get_battery() const
{
    std::lock_guard<std::mutex> lock(_battery_mutex);
    return _battery;
}

void TelemetryImpl::set_battery(Telemetry::Battery battery)
{
    std::lock_guard<std::mutex> lock(_battery_mutex);
    _battery = battery;
}

Telemetry::FlightMode TelemetryImpl::get_flight_mode() const
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    return _flight_mode;
}

void TelemetryImpl::set_flight_mode(Telemetry::FlightMode flight_mode)
{
    std::lock_guard<std::mutex> lock(_flight_mode_mutex);
    _flight_mode = flight_mode;
}

Telemetry::Health TelemetryImpl::get_health() const
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    return _health;
}

bool TelemetryImpl::get_health_all_ok() const
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    if (_health.gyrometer_calibration_ok &&
        _health.accelerometer_calibration_ok &&
        _health.magnetometer_calibration_ok &&
        _health.level_calibration_ok &&
        _health.local_position_ok &&
        _health.global_position_ok &&
        _health.home_position_ok) {
        return true;
    } else {
        return false;
    }
}

Telemetry::RCStatus TelemetryImpl::get_rc_status() const
{
    std::lock_guard<std::mutex> lock(_rc_status_mutex);
    return _rc_status;
}

void TelemetryImpl::set_health_local_position(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.local_position_ok = ok;
}

void TelemetryImpl::set_health_global_position(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.global_position_ok = ok;
}

void TelemetryImpl::set_health_home_position(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.home_position_ok = ok;
}

void TelemetryImpl::set_health_gyrometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.gyrometer_calibration_ok = ok;
}

void TelemetryImpl::set_health_accelerometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.accelerometer_calibration_ok = ok;
}

void TelemetryImpl::set_health_magnetometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.magnetometer_calibration_ok = ok;
}

void TelemetryImpl::set_health_level_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.level_calibration_ok = ok;
}

void TelemetryImpl::set_rc_status(bool available, float signal_strength_percent)
{
    std::lock_guard<std::mutex> lock(_rc_status_mutex);

    if (available) {
        _rc_status.available_once = true;
        _rc_status.signal_strength_percent = signal_strength_percent;
    } else {
        _rc_status.signal_strength_percent = 0.0f;
    }

    _rc_status.available = available;

}

void TelemetryImpl::position_async(Telemetry::position_callback_t &callback)
{
    _position_subscription = callback;
}

void TelemetryImpl::home_position_async(Telemetry::position_callback_t &callback)
{
    _home_position_subscription = callback;
}

void TelemetryImpl::in_air_async(Telemetry::in_air_callback_t &callback)
{
    _in_air_subscription = callback;
}

void TelemetryImpl::armed_async(Telemetry::armed_callback_t &callback)
{
    _armed_subscription = callback;
}

void TelemetryImpl::attitude_quaternion_async(Telemetry::attitude_quaternion_callback_t &callback)
{
    _attitude_quaternion_subscription = callback;
}

void TelemetryImpl::attitude_euler_angle_async(Telemetry::attitude_euler_angle_callback_t
                                               &callback)
{
    _attitude_euler_angle_subscription = callback;
}

void TelemetryImpl::camera_attitude_quaternion_async(Telemetry::attitude_quaternion_callback_t
                                                     &callback)
{
    _camera_attitude_quaternion_subscription = callback;
}

void TelemetryImpl::camera_attitude_euler_angle_async(Telemetry::attitude_euler_angle_callback_t
                                                      &callback)
{
    _camera_attitude_euler_angle_subscription = callback;
}

void TelemetryImpl::ground_speed_ned_async(Telemetry::ground_speed_ned_callback_t &callback)
{
    _ground_speed_ned_subscription = callback;
}

void TelemetryImpl::gps_info_async(Telemetry::gps_info_callback_t &callback)
{
    _gps_info_subscription = callback;
}

void TelemetryImpl::battery_async(Telemetry::battery_callback_t &callback)
{
    _battery_subscription = callback;
}

void TelemetryImpl::flight_mode_async(Telemetry::flight_mode_callback_t &callback)
{
    _flight_mode_subscription = callback;
}

void TelemetryImpl::health_async(Telemetry::health_callback_t &callback)
{
    _health_subscription = callback;
}

void TelemetryImpl::health_all_ok_async(Telemetry::health_all_ok_callback_t &callback)
{
    _health_all_ok_subscription = callback;
}

void TelemetryImpl::rc_status_async(Telemetry::rc_status_callback_t &callback)
{
    _rc_status_subscription = callback;
}

} // namespace dronecore
