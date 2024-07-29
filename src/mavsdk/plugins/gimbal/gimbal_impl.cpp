#include "gimbal_impl.h"
#include "callback_list.tpp"
#include "mavsdk_math.h"
#include "math_conversions.h"
#include <chrono>
#include <cmath>
#include <functional>
#include <thread>

namespace mavsdk {

template class CallbackList<Gimbal::ControlStatus>;

GimbalImpl::GimbalImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

GimbalImpl::GimbalImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

GimbalImpl::~GimbalImpl()
{
    _system_impl->unregister_plugin(this);
}

void GimbalImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION,
        [this](const mavlink_message_t& message) { process_gimbal_manager_information(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS,
        [this](const mavlink_message_t& message) { process_gimbal_manager_status(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS,
        [this](const mavlink_message_t& message) {
            process_gimbal_device_attitude_status(message);
        },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ATTITUDE,
        [this](const mavlink_message_t& message) { process_attitude(message); },
        this);
}

void GimbalImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers(this);
}

void GimbalImpl::enable()
{
    request_gimbal_information();
}

void GimbalImpl::disable() {}

void GimbalImpl::request_gimbal_information()
{
    MavlinkCommandSender::CommandLong command{};
    command.command = MAV_CMD_REQUEST_MESSAGE;
    command.params.maybe_param1 = {static_cast<float>(MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION)};
    command.target_component_id = 0; // any component
    _system_impl->send_command_async(command, nullptr);
}

void GimbalImpl::process_gimbal_manager_information(const mavlink_message_t& message)
{
    mavlink_gimbal_manager_information_t gimbal_manager_information;
    mavlink_msg_gimbal_manager_information_decode(&message, &gimbal_manager_information);

    // TODO: implement
}

void GimbalImpl::process_gimbal_manager_status(const mavlink_message_t& message)
{
    mavlink_gimbal_manager_status_t status;
    mavlink_msg_gimbal_manager_status_decode(&message, &status);

    std::lock_guard<std::mutex> lock(_mutex);

    if (status.primary_control_sysid == static_cast<int>(_system_impl->get_own_system_id()) &&
        status.primary_control_compid == static_cast<int>(_system_impl->get_own_component_id())) {
        _control_status.control_mode = Gimbal::ControlMode::Primary;
    } else if (
        status.secondary_control_sysid == static_cast<int>(_system_impl->get_own_system_id()) &&
        status.secondary_control_compid == static_cast<int>(_system_impl->get_own_component_id())) {
        _control_status.control_mode = Gimbal::ControlMode::Secondary;
    } else {
        _control_status.control_mode = Gimbal::ControlMode::None;
    }

    _control_status.sysid_primary_control = status.primary_control_sysid;
    _control_status.compid_primary_control = status.primary_control_compid;
    _control_status.sysid_secondary_control = status.secondary_control_sysid;
    _control_status.compid_secondary_control = status.secondary_control_compid;

    _control_subscriptions.queue(
        _control_status, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void GimbalImpl::process_gimbal_device_attitude_status(const mavlink_message_t& message)
{
    mavlink_gimbal_device_attitude_status_t attitude_status;
    mavlink_msg_gimbal_device_attitude_status_decode(&message, &attitude_status);

    // By default, we assume it's in vehicle/forward frame.
    bool is_in_forward_frame = true;

    if (attitude_status.flags & GIMBAL_DEVICE_FLAGS_YAW_IN_VEHICLE_FRAME ||
        attitude_status.flags & GIMBAL_DEVICE_FLAGS_YAW_IN_EARTH_FRAME) {
        // Flags are set correctly according to newer spec, so we can use it.
        if (attitude_status.flags & GIMBAL_DEVICE_FLAGS_YAW_IN_EARTH_FRAME) {
            is_in_forward_frame = false;
        }
    } else {
        // Neither of the flags indicating the frame are set, we fallback to previous way
        // which depends on lock flag.
        if (attitude_status.flags & GIMBAL_DEVICE_FLAGS_YAW_LOCK) {
            is_in_forward_frame = false;
        }
    }

    std::lock_guard<std::mutex> lock(_mutex);

    // Reset to defaults (e.g. NaN) first.
    _attitude = {};

    if (is_in_forward_frame) {
        _attitude.quaternion_forward.w = attitude_status.q[0];
        _attitude.quaternion_forward.x = attitude_status.q[1];
        _attitude.quaternion_forward.y = attitude_status.q[2];
        _attitude.quaternion_forward.z = attitude_status.q[3];

        auto quaternion_forward = Quaternion{};
        quaternion_forward.w = attitude_status.q[0];
        quaternion_forward.x = attitude_status.q[1];
        quaternion_forward.y = attitude_status.q[2];
        quaternion_forward.z = attitude_status.q[3];
        const auto euler_angle_forward = to_euler_angle_from_quaternion(quaternion_forward);

        _attitude.euler_angle_forward.roll_deg = euler_angle_forward.roll_deg;
        _attitude.euler_angle_forward.pitch_deg = euler_angle_forward.pitch_deg;
        _attitude.euler_angle_forward.yaw_deg = euler_angle_forward.yaw_deg;

        _attitude.timestamp_us = attitude_status.time_boot_ms * 1000;

        // Calculate angle relative to North as well
        if (!std::isnan(_vehicle_yaw_rad)) {
            auto rotation =
                to_quaternion_from_euler_angle(EulerAngle{0, 0, to_deg_from_rad(_vehicle_yaw_rad)});
            auto quaternion_north = rotation * quaternion_forward;

            _attitude.quaternion_north.w = quaternion_north.w;
            _attitude.quaternion_north.x = quaternion_north.x;
            _attitude.quaternion_north.y = quaternion_north.y;
            _attitude.quaternion_north.z = quaternion_north.z;

            const auto euler_angle_north = to_euler_angle_from_quaternion(quaternion_north);
            _attitude.euler_angle_north.roll_deg = euler_angle_north.roll_deg;
            _attitude.euler_angle_north.pitch_deg = euler_angle_north.pitch_deg;
            _attitude.euler_angle_north.yaw_deg = euler_angle_north.yaw_deg;
        }

    } else {
        _attitude.quaternion_north.w = attitude_status.q[0];
        _attitude.quaternion_north.x = attitude_status.q[1];
        _attitude.quaternion_north.y = attitude_status.q[2];
        _attitude.quaternion_north.z = attitude_status.q[3];

        auto quaternion_north = Quaternion{};
        quaternion_north.w = attitude_status.q[0];
        quaternion_north.x = attitude_status.q[1];
        quaternion_north.y = attitude_status.q[2];
        quaternion_north.z = attitude_status.q[3];
        const auto euler_angle_north = to_euler_angle_from_quaternion(quaternion_north);

        _attitude.euler_angle_north.roll_deg = euler_angle_north.roll_deg;
        _attitude.euler_angle_north.pitch_deg = euler_angle_north.pitch_deg;
        _attitude.euler_angle_north.yaw_deg = euler_angle_north.yaw_deg;

        // Calculate angle relative to forward as well
        if (!std::isnan(_vehicle_yaw_rad)) {
            auto rotation = to_quaternion_from_euler_angle(
                EulerAngle{0, 0, -to_deg_from_rad(_vehicle_yaw_rad)});
            auto quaternion_forward = rotation * quaternion_north;

            _attitude.quaternion_forward.w = quaternion_forward.w;
            _attitude.quaternion_forward.x = quaternion_forward.x;
            _attitude.quaternion_forward.y = quaternion_forward.y;
            _attitude.quaternion_forward.z = quaternion_forward.z;

            const auto euler_angle_forward = to_euler_angle_from_quaternion(quaternion_forward);
            _attitude.euler_angle_forward.roll_deg = euler_angle_forward.roll_deg;
            _attitude.euler_angle_forward.pitch_deg = euler_angle_forward.pitch_deg;
            _attitude.euler_angle_forward.yaw_deg = euler_angle_forward.yaw_deg;
        }
    }

    _attitude.angular_velocity.roll_rad_s = attitude_status.angular_velocity_x;
    _attitude.angular_velocity.pitch_rad_s = attitude_status.angular_velocity_y;
    _attitude.angular_velocity.yaw_rad_s = attitude_status.angular_velocity_z;

    _attitude_subscriptions.queue(
        _attitude, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void GimbalImpl::process_attitude(const mavlink_message_t& message)
{
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&message, &attitude);

    std::lock_guard<std::mutex> lock(_mutex);

    _vehicle_yaw_rad = attitude.yaw;
}

Gimbal::Result GimbalImpl::set_angles(float roll_deg, float pitch_deg, float yaw_deg)
{
    const float roll_rad = to_rad_from_deg(roll_deg);
    const float pitch_rad = to_rad_from_deg(pitch_deg);
    const float yaw_rad = to_rad_from_deg(yaw_deg);

    float quaternion[4];
    mavlink_euler_to_quaternion(roll_rad, pitch_rad, yaw_rad, quaternion);

    std::lock_guard<std::mutex> lock(_mutex);

    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((_gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_gimbal_manager_set_attitude_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _gimbal_manager_sysid,
            _gimbal_manager_compid,
            flags,
            _gimbal_device_id,
            quaternion,
            NAN,
            NAN,
            NAN);
        return message;
    }) ?
               Gimbal::Result::Success :
               Gimbal::Result::Error;
}

void GimbalImpl::set_angles_async(
    float roll_deg, float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    Gimbal::Result result = set_angles(roll_deg, pitch_deg, yaw_deg);

    std::lock_guard<std::mutex> lock(_mutex);

    if (callback) {
        _system_impl->call_user_callback([callback, result]() { callback(result); });
    }
}

Gimbal::Result GimbalImpl::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    return set_angles(0.0f, pitch_deg, yaw_deg);
}

void GimbalImpl::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    set_angles_async(0.0f, pitch_deg, yaw_deg, callback);
}

Gimbal::Result GimbalImpl::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((_gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    const float quaternion[4] = {NAN, NAN, NAN, NAN};

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_gimbal_manager_set_attitude_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _gimbal_manager_sysid,
            _gimbal_manager_compid,
            flags,
            _gimbal_device_id,
            quaternion,
            0.0f,
            to_rad_from_deg(pitch_rate_deg_s),
            to_rad_from_deg(yaw_rate_deg_s));
        return message;
    }) ?
               Gimbal::Result::Success :
               Gimbal::Result::Error;
}

void GimbalImpl::set_pitch_rate_and_yaw_rate_async(
    float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    Gimbal::Result result = set_pitch_rate_and_yaw_rate(pitch_rate_deg_s, yaw_rate_deg_s);

    std::lock_guard<std::mutex> lock(_mutex);

    if (callback) {
        auto temp_callback = callback;
        _system_impl->call_user_callback([temp_callback, result]() { temp_callback(result); });
    }
}

Gimbal::Result GimbalImpl::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _gimbal_mode = gimbal_mode;
    return Gimbal::Result::Success;
}

void GimbalImpl::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _gimbal_mode = gimbal_mode;

    if (callback) {
        _system_impl->call_user_callback([callback]() { callback(Gimbal::Result::Success); });
    }
}

Gimbal::Result
GimbalImpl::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    return gimbal_result_from_command_result(_system_impl->send_command(command));
}

void GimbalImpl::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    _system_impl->send_command_async(
        command, [callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

Gimbal::Result GimbalImpl::take_control(Gimbal::ControlMode control_mode)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    take_control_async(control_mode, [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalImpl::take_control_async(
    Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (control_mode == Gimbal::ControlMode::None) {
        release_control_async(callback);
        return;
    } else if (control_mode == Gimbal::ControlMode::Secondary) {
        _system_impl->call_user_callback([callback]() { callback(Gimbal::Result::Unsupported); });
        LogErr() << "Gimbal secondary control is not implemented yet!";
        return;
    }

    float own_sysid = _system_impl->get_own_system_id();
    float own_compid = _system_impl->get_own_component_id();

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE;
    command.params.maybe_param1 =
        control_mode == Gimbal::ControlMode::Primary ? own_sysid : -3.0f; // sysid primary control
    command.params.maybe_param2 =
        control_mode == Gimbal::ControlMode::Primary ? own_compid : -3.0f; // compid primary control
    command.params.maybe_param3 =
        control_mode == Gimbal::ControlMode::Primary ? own_sysid : -3.0f; // sysid secondary control
    command.params.maybe_param4 = control_mode == Gimbal::ControlMode::Primary ?
                                      own_compid :
                                      -3.0f; // compid secondary control

    command.params.maybe_param7 = _gimbal_device_id;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    _system_impl->send_command_async(
        command, [callback](MavlinkCommandSender::Result result, float) {
            GimbalImpl::receive_command_result(result, callback);
        });
}

Gimbal::Result GimbalImpl::release_control()
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    release_control_async([&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalImpl::release_control_async(Gimbal::ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE;
    command.params.maybe_param1 = -3.0f; // sysid primary control
    command.params.maybe_param2 = -3.0f; // compid primary control
    command.params.maybe_param3 = -3.0f; // sysid secondary control
    command.params.maybe_param4 = -3.0f; // compid secondary control
    command.params.maybe_param7 = _gimbal_device_id;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    _system_impl->send_command_async(
        command, [callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

Gimbal::ControlStatus GimbalImpl::control()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _control_status;
}

Gimbal::ControlHandle GimbalImpl::subscribe_control(const Gimbal::ControlCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _control_subscriptions.subscribe(callback);
}

void GimbalImpl::unsubscribe_control(Gimbal::ControlHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _control_subscriptions.unsubscribe(handle);
}

Gimbal::AttitudeHandle GimbalImpl::subscribe_attitude(const Gimbal::AttitudeCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _attitude_subscriptions.subscribe(callback);
}

void GimbalImpl::unsubscribe_attitude(Gimbal::AttitudeHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _attitude_subscriptions.unsubscribe(handle);
}

Gimbal::Attitude GimbalImpl::attitude()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _attitude;
}

void GimbalImpl::receive_command_result(
    MavlinkCommandSender::Result command_result, const Gimbal::ResultCallback& callback)
{
    Gimbal::Result gimbal_result = gimbal_result_from_command_result(command_result);

    if (callback) {
        callback(gimbal_result);
    }
}

Gimbal::Result
GimbalImpl::gimbal_result_from_command_result(MavlinkCommandSender::Result command_result)
{
    switch (command_result) {
        case MavlinkCommandSender::Result::Success:
            return Gimbal::Result::Success;
        case MavlinkCommandSender::Result::Timeout:
            return Gimbal::Result::Timeout;
        case MavlinkCommandSender::Result::NoSystem:
        case MavlinkCommandSender::Result::ConnectionError:
        case MavlinkCommandSender::Result::Busy:
        case MavlinkCommandSender::Result::Denied:
        case MavlinkCommandSender::Result::TemporarilyRejected:
        default:
            return Gimbal::Result::Error;
    }
}

} // namespace mavsdk
