#include "gimbal_impl.h"
#include "callback_list.tpp"
#include "mavsdk_math.h"
#include "math_conversions.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <functional>

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
    if (const char* env_p = std::getenv("MAVSDK_GIMBAL_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Gimbal debugging is on.";
            _debugging = true;
        }
    }

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) { process_heartbeat(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION,
        [this](const mavlink_message_t& message) { process_gimbal_manager_information(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS,
        [this](const mavlink_message_t& message) { process_gimbal_manager_status(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION,
        [this](const mavlink_message_t& message) { process_gimbal_device_information(message); },
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

void GimbalImpl::enable() {}

void GimbalImpl::disable() {}

void GimbalImpl::request_gimbal_manager_information(uint8_t target_component_id) const
{
    if (_debugging) {
        LogDebug() << "Requesting GIMBAL_MANAGER_INFORMATION from: "
                   << std::to_string(_system_impl->get_system_id()) << "/"
                   << std::to_string(target_component_id);
    }

    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION, target_component_id, nullptr);
}

void GimbalImpl::request_gimbal_device_information(uint8_t target_component_id) const
{
    if (_debugging) {
        LogDebug() << "Requesting GIMBAL_DEVICE_INFORMATION from: "
                   << std::to_string(_system_impl->get_system_id()) << "/"
                   << std::to_string(target_component_id);
    }

    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION, target_component_id, nullptr);
}

void GimbalImpl::process_heartbeat(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_gimbal = std::find_if(_gimbals.begin(), _gimbals.end(), [&](const GimbalItem& item) {
        return item.gimbal_manager_compid == message.compid;
    });

    // Every component can potentially be a gimbal manager. Therefore, on any
    // heartbeat arriving, we create an entry in the potential gimbal manager
    // list and subsequently try to figure out whether it sends gimbal manager
    // messages.
    auto* gimbal = [&]() {
        if (maybe_gimbal != _gimbals.end()) {
            // Going from iterator to pointer is not exactly pretty.
            return &(*maybe_gimbal);
        } else {
            GimbalItem new_item{};
            new_item.gimbal_manager_compid = message.compid;
            _gimbals.emplace_back(std::move(new_item));

            return &_gimbals.back();
        }
    }();

    check_is_gimbal_valid(gimbal);
}

void GimbalImpl::process_gimbal_manager_information(const mavlink_message_t& message)
{
    mavlink_gimbal_manager_information_t gimbal_manager_information;
    mavlink_msg_gimbal_manager_information_decode(&message, &gimbal_manager_information);

    if (_debugging) {
        LogDebug() << "Got GIMBAL_MANAGER_INFORMATION from: " << std::to_string(message.sysid)
                   << "/" << std::to_string(message.compid) << " with gimbal_device_id: "
                   << std::to_string(gimbal_manager_information.gimbal_device_id);
    }

    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_gimbal = std::find_if(_gimbals.begin(), _gimbals.end(), [&](const GimbalItem& item) {
        return item.gimbal_manager_compid == message.compid;
    });

    auto* gimbal = [&]() {
        if (maybe_gimbal != _gimbals.end()) {
            // Going from iterator to pointer is not exactly pretty.
            return &(*maybe_gimbal);
        } else {
            GimbalItem new_item{};
            new_item.gimbal_manager_compid = message.compid;
            new_item.gimbal_device_id = gimbal_manager_information.gimbal_device_id;
            _gimbals.emplace_back(new_item);
            return &_gimbals.back();
        }
    }();

    if (gimbal->gimbal_manager_information_received &&
        gimbal->gimbal_device_id != gimbal_manager_information.gimbal_device_id) {
        LogWarn() << "gimbal_manager_information.gimbal_device_id changed from "
                  << gimbal->gimbal_device_id << " to "
                  << gimbal_manager_information.gimbal_device_id;
    }
    gimbal->gimbal_device_id = gimbal_manager_information.gimbal_device_id;
    gimbal->gimbal_manager_information_received = true;

    check_is_gimbal_valid(gimbal);
}

void GimbalImpl::process_gimbal_manager_status(const mavlink_message_t& message)
{
    mavlink_gimbal_manager_status_t status;
    mavlink_msg_gimbal_manager_status_decode(&message, &status);

    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_gimbal = std::find_if(_gimbals.begin(), _gimbals.end(), [&](const GimbalItem& item) {
        return item.gimbal_manager_compid == message.compid &&
               item.gimbal_device_id == status.gimbal_device_id;
    });

    if (maybe_gimbal == _gimbals.end()) {
        // No potential entry exists yet, we just give up for now.
        return;
    }

    auto& gimbal = *maybe_gimbal;

    // We need to populate the MAVSDK gimbal ID, so the user knows which is which.
    // +1 because 0 means all, so it's one-based.
    gimbal.control_status.gimbal_id =
        static_cast<int32_t>(std::distance(_gimbals.begin(), maybe_gimbal)) + 1;

    if (status.primary_control_sysid == static_cast<int>(_system_impl->get_own_system_id()) &&
        status.primary_control_compid == static_cast<int>(_system_impl->get_own_component_id())) {
        gimbal.control_status.control_mode = Gimbal::ControlMode::Primary;
    } else if (
        status.secondary_control_sysid == static_cast<int>(_system_impl->get_own_system_id()) &&
        status.secondary_control_compid == static_cast<int>(_system_impl->get_own_component_id())) {
        gimbal.control_status.control_mode = Gimbal::ControlMode::Secondary;
    } else {
        gimbal.control_status.control_mode = Gimbal::ControlMode::None;
    }

    gimbal.control_status.sysid_primary_control = status.primary_control_sysid;
    gimbal.control_status.compid_primary_control = status.primary_control_compid;
    gimbal.control_status.sysid_secondary_control = status.secondary_control_sysid;
    gimbal.control_status.compid_secondary_control = status.secondary_control_compid;

    _control_status_subscriptions.queue(gimbal.control_status, [this](const auto& func) {
        _system_impl->call_user_callback(func);
    });
}

void GimbalImpl::process_gimbal_device_information(const mavlink_message_t& message)
{
    mavlink_gimbal_device_information_t gimbal_device_information;
    mavlink_msg_gimbal_device_information_decode(&message, &gimbal_device_information);

    if (_debugging) {
        LogDebug() << "Got GIMBAL_DEVICE_INFORMATION from: " << std::to_string(message.sysid) << "/"
                   << std::to_string(message.compid) << " with gimbal_device_id: "
                   << std::to_string(gimbal_device_information.gimbal_device_id);
    }

    auto maybe_gimbal = std::find_if(_gimbals.begin(), _gimbals.end(), [&](const GimbalItem& item) {
        if (gimbal_device_information.gimbal_device_id == 0) {
            return item.gimbal_device_id == message.compid;
        } else {
            return item.gimbal_manager_compid == message.compid;
        }
    });

    if (maybe_gimbal == _gimbals.end()) {
        if (_debugging) {
            LogDebug() << "Didn't find gimbal for gimbal device";
        }
        return;
    }
    auto gimbal = &(*maybe_gimbal);

    gimbal->gimbal_device_information_received = true;
    gimbal->vendor_name = gimbal_device_information.vendor_name;
    gimbal->model_name = gimbal_device_information.model_name;
    gimbal->custom_name = gimbal_device_information.custom_name;

    check_is_gimbal_valid(gimbal);
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
        // Neither of the flags indicating the frame are set, we fall back to previous way
        // which depends on lock flag.
        if (attitude_status.flags & GIMBAL_DEVICE_FLAGS_YAW_LOCK) {
            is_in_forward_frame = false;
        }
    }

    if (attitude_status.gimbal_device_id > 6) {
        LogWarn() << "Ignoring gimbal device attitude status with invalid gimbal_device_id "
                  << attitude_status.gimbal_device_id << " from (" << message.sysid << "/" << ")";
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_gimbal = std::find_if(_gimbals.begin(), _gimbals.end(), [&](const GimbalItem& item) {
        if (attitude_status.gimbal_device_id == 0) {
            return item.gimbal_device_id == message.compid;
        } else {
            return item.gimbal_manager_compid == message.compid &&
                   item.gimbal_device_id == attitude_status.gimbal_device_id;
        }
    });

    if (maybe_gimbal == _gimbals.end()) {
        // Only warn if we have found any gimbals.
        if (std::any_of(_gimbals.begin(), _gimbals.end(), [](const GimbalItem& item) {
                return item.is_valid;
            })) {
            LogWarn() << "Received gimbal manager status for unknown gimbal.";
        }
        // Otherwise, ignore it silently
        return;
    }

    auto& gimbal = *maybe_gimbal;

    // Reset to defaults (e.g. NaN) first.
    gimbal.attitude = {};
    // We need to populate the MAVSDK gimbal ID, so the user knows which is which.
    // +1 because 0 means all, so it's one-based.
    gimbal.attitude.gimbal_id =
        static_cast<int32_t>(std::distance(_gimbals.begin(), maybe_gimbal)) + 1;

    if (is_in_forward_frame) {
        gimbal.attitude.quaternion_forward.w = attitude_status.q[0];
        gimbal.attitude.quaternion_forward.x = attitude_status.q[1];
        gimbal.attitude.quaternion_forward.y = attitude_status.q[2];
        gimbal.attitude.quaternion_forward.z = attitude_status.q[3];

        auto quaternion_forward = Quaternion{};
        quaternion_forward.w = attitude_status.q[0];
        quaternion_forward.x = attitude_status.q[1];
        quaternion_forward.y = attitude_status.q[2];
        quaternion_forward.z = attitude_status.q[3];
        const auto euler_angle_forward = to_euler_angle_from_quaternion(quaternion_forward);

        gimbal.attitude.euler_angle_forward.roll_deg = euler_angle_forward.roll_deg;
        gimbal.attitude.euler_angle_forward.pitch_deg = euler_angle_forward.pitch_deg;
        gimbal.attitude.euler_angle_forward.yaw_deg = euler_angle_forward.yaw_deg;

        gimbal.attitude.timestamp_us = attitude_status.time_boot_ms * 1000;

        // Calculate angle relative to North as well
        if (!std::isnan(_vehicle_yaw_rad)) {
            auto rotation =
                to_quaternion_from_euler_angle(EulerAngle{0, 0, to_deg_from_rad(_vehicle_yaw_rad)});
            auto quaternion_north = rotation * quaternion_forward;

            gimbal.attitude.quaternion_north.w = quaternion_north.w;
            gimbal.attitude.quaternion_north.x = quaternion_north.x;
            gimbal.attitude.quaternion_north.y = quaternion_north.y;
            gimbal.attitude.quaternion_north.z = quaternion_north.z;

            const auto euler_angle_north = to_euler_angle_from_quaternion(quaternion_north);
            gimbal.attitude.euler_angle_north.roll_deg = euler_angle_north.roll_deg;
            gimbal.attitude.euler_angle_north.pitch_deg = euler_angle_north.pitch_deg;
            gimbal.attitude.euler_angle_north.yaw_deg = euler_angle_north.yaw_deg;
        }

    } else {
        gimbal.attitude.quaternion_north.w = attitude_status.q[0];
        gimbal.attitude.quaternion_north.x = attitude_status.q[1];
        gimbal.attitude.quaternion_north.y = attitude_status.q[2];
        gimbal.attitude.quaternion_north.z = attitude_status.q[3];

        auto quaternion_north = Quaternion{};
        quaternion_north.w = attitude_status.q[0];
        quaternion_north.x = attitude_status.q[1];
        quaternion_north.y = attitude_status.q[2];
        quaternion_north.z = attitude_status.q[3];
        const auto euler_angle_north = to_euler_angle_from_quaternion(quaternion_north);

        gimbal.attitude.euler_angle_north.roll_deg = euler_angle_north.roll_deg;
        gimbal.attitude.euler_angle_north.pitch_deg = euler_angle_north.pitch_deg;
        gimbal.attitude.euler_angle_north.yaw_deg = euler_angle_north.yaw_deg;

        // Calculate angle relative to forward as well
        if (!std::isnan(_vehicle_yaw_rad)) {
            auto rotation = to_quaternion_from_euler_angle(
                EulerAngle{0, 0, -to_deg_from_rad(_vehicle_yaw_rad)});
            auto quaternion_forward = rotation * quaternion_north;

            gimbal.attitude.quaternion_forward.w = quaternion_forward.w;
            gimbal.attitude.quaternion_forward.x = quaternion_forward.x;
            gimbal.attitude.quaternion_forward.y = quaternion_forward.y;
            gimbal.attitude.quaternion_forward.z = quaternion_forward.z;

            const auto euler_angle_forward = to_euler_angle_from_quaternion(quaternion_forward);
            gimbal.attitude.euler_angle_forward.roll_deg = euler_angle_forward.roll_deg;
            gimbal.attitude.euler_angle_forward.pitch_deg = euler_angle_forward.pitch_deg;
            gimbal.attitude.euler_angle_forward.yaw_deg = euler_angle_forward.yaw_deg;
        }
    }

    gimbal.attitude.angular_velocity.roll_rad_s = attitude_status.angular_velocity_x;
    gimbal.attitude.angular_velocity.pitch_rad_s = attitude_status.angular_velocity_y;
    gimbal.attitude.angular_velocity.yaw_rad_s = attitude_status.angular_velocity_z;

    _attitude_subscriptions.queue(
        gimbal.attitude, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void GimbalImpl::process_attitude(const mavlink_message_t& message)
{
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&message, &attitude);

    std::lock_guard<std::mutex> lock(_mutex);

    _vehicle_yaw_rad = attitude.yaw;
}

void GimbalImpl::check_is_gimbal_valid(GimbalItem* gimbal)
{
    assert(gimbal != nullptr);

    // Assumes lock

    if (gimbal->is_valid) {
        // We're already done.
        return;
    }

    // Check if we should request GIMBAL_MANAGER_INFORMATION again.
    if (!gimbal->gimbal_manager_information_received &&
        gimbal->gimbal_manager_information_requests_left-- > 0) {
        request_gimbal_manager_information(gimbal->gimbal_manager_compid);
    }

    if (!gimbal->gimbal_manager_information_received) {
        // Leave it at this for now, we need the gimbal_device_id for further steps
        return;
    }

    // Check if we should request GIMBAL_DEVICE_INFORMATION again.
    if (!gimbal->gimbal_device_information_received &&
        gimbal->gimbal_device_information_requests_left-- > 0) {
        auto component_id = (gimbal->gimbal_device_id > 0 && gimbal->gimbal_device_id <= 6) ?
                                gimbal->gimbal_manager_compid :
                                gimbal->gimbal_device_id;
        if (component_id != 0) {
            request_gimbal_device_information(component_id);
        }
        return;
    }

    // If we have gimbal_manager_information but no GIMBAL_DEVICE_INFORMATION despite
    // having tried multiple times, we might as well continue without.
    if (!gimbal->gimbal_device_information_received) {
        LogWarn() << "Continuing despite GIMBAL_DEVICE_INFORMATION missing";
    }

    gimbal->is_valid = true;
    _gimbal_list_subscriptions.queue(gimbal_list_with_lock(), [this](const auto& func) {
        _system_impl->call_user_callback(func);
    });
}

Gimbal::Result GimbalImpl::set_angles(
    int32_t gimbal_id,
    float roll_deg,
    float pitch_deg,
    float yaw_deg,
    Gimbal::GimbalMode gimbal_mode,
    Gimbal::SendMode send_mode)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    set_angles_async_internal(
        gimbal_id,
        roll_deg,
        pitch_deg,
        yaw_deg,
        gimbal_mode,
        send_mode,
        [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalImpl::set_angles_async(
    int32_t gimbal_id,
    float roll_deg,
    float pitch_deg,
    float yaw_deg,
    Gimbal::GimbalMode gimbal_mode,
    Gimbal::SendMode send_mode,
    const Gimbal::ResultCallback& callback)
{
    set_angles_async_internal(
        gimbal_id,
        roll_deg,
        pitch_deg,
        yaw_deg,
        gimbal_mode,
        send_mode,
        [this, callback](auto result) {
            if (callback) {
                _system_impl->call_user_callback([callback, result]() { callback(result); });
            }
        });
}

void GimbalImpl::set_angles_async_internal(
    int32_t gimbal_id,
    float roll_deg,
    float pitch_deg,
    float yaw_deg,
    Gimbal::GimbalMode gimbal_mode,
    Gimbal::SendMode send_mode,
    const Gimbal::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_address = maybe_address_for_gimbal_id(gimbal_id);

    if (!maybe_address) {
        if (callback) {
            callback(Gimbal::Result::InvalidArgument);
        }
        return;
    }

    auto address = maybe_address.value();

    const float roll_rad = to_rad_from_deg(roll_deg);
    const float pitch_rad = to_rad_from_deg(pitch_deg);
    const float yaw_rad = to_rad_from_deg(yaw_deg);

    float quaternion[4];
    mavlink_euler_to_quaternion(roll_rad, pitch_rad, yaw_rad, quaternion);

    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    switch (send_mode) {
        case Gimbal::SendMode::Stream: {
            if (_debugging) {
                LogDebug() << "Sending GIMBAL_MANAGER_SET_ATTITUDE message with angles";
            }
            auto result =
                _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
                    mavlink_message_t message;
                    mavlink_msg_gimbal_manager_set_attitude_pack_chan(
                        mavlink_address.system_id,
                        mavlink_address.component_id,
                        channel,
                        &message,
                        _system_impl->get_system_id(),
                        address.gimbal_manager_compid,
                        flags,
                        address.gimbal_device_id,
                        quaternion,
                        NAN,
                        NAN,
                        NAN);
                    return message;
                }) ?
                    Gimbal::Result::Success :
                    Gimbal::Result::Error;
            if (callback) {
                callback(result);
            }
        } break;
        case Gimbal::SendMode::Once:
            if (roll_deg != 0.0f) {
                LogWarn() << "Roll needs to be 0 for SendMode::Once.";
                if (callback) {
                    callback(Gimbal::Result::InvalidArgument);
                }

            } else {
                if (_debugging) {
                    LogDebug() << "Sending DO_GIMBAL_MANAGER_PITCHYAW command with angles";
                }

                MavlinkCommandSender::CommandLong command{};
                command.command = MAV_CMD_DO_GIMBAL_MANAGER_PITCHYAW;
                command.params.maybe_param1 = pitch_deg;
                command.params.maybe_param2 = yaw_deg;
                command.params.maybe_param3 = NAN;
                command.params.maybe_param4 = NAN;
                command.params.maybe_param5 = static_cast<float>(flags);
                command.params.maybe_param7 = address.gimbal_device_id;
                command.target_system_id = _system_impl->get_system_id();
                command.target_component_id = address.gimbal_manager_compid;

                _system_impl->send_command_async(
                    command, [callback](MavlinkCommandSender::Result result, float) {
                        receive_command_result(result, callback);
                    });
            }
            break;
        default:
            LogErr() << "Invalid send mode";
            if (callback) {
                callback(Gimbal::Result::InvalidArgument);
            }
            break;
    }
}

Gimbal::Result GimbalImpl::set_angular_rates(
    int32_t gimbal_id,
    float roll_rate_deg_s,
    float pitch_rate_deg_s,
    float yaw_rate_deg_s,
    Gimbal::GimbalMode gimbal_mode,
    Gimbal::SendMode send_mode)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    set_angular_rates_async_internal(
        gimbal_id,
        roll_rate_deg_s,
        pitch_rate_deg_s,
        yaw_rate_deg_s,
        gimbal_mode,
        send_mode,
        [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalImpl::set_angular_rates_async(
    int32_t gimbal_id,
    float roll_rate_deg_s,
    float pitch_rate_deg_s,
    float yaw_rate_deg_s,
    Gimbal::GimbalMode gimbal_mode,
    Gimbal::SendMode send_mode,
    const Gimbal::ResultCallback& callback)
{
    set_angular_rates_async_internal(
        gimbal_id,
        roll_rate_deg_s,
        pitch_rate_deg_s,
        yaw_rate_deg_s,
        gimbal_mode,
        send_mode,
        [this, callback](auto result) {
            if (callback) {
                _system_impl->call_user_callback([callback, result]() { callback(result); });
            }
        });
}

void GimbalImpl::set_angular_rates_async_internal(
    int32_t gimbal_id,
    float roll_rate_deg_s,
    float pitch_rate_deg_s,
    float yaw_rate_deg_s,
    Gimbal::GimbalMode gimbal_mode,
    Gimbal::SendMode send_mode,
    const Gimbal::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_address = maybe_address_for_gimbal_id(gimbal_id);

    if (!maybe_address) {
        if (callback) {
            callback(Gimbal::Result::InvalidArgument);
        }
        return;
    }

    auto address = maybe_address.value();

    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    switch (send_mode) {
        case Gimbal::SendMode::Stream: {
            constexpr float quaternion[4] = {NAN, NAN, NAN, NAN};

            if (_debugging) {
                LogDebug() << "Sending GIMBAL_MANAGER_SET_ATTITUDE message with angular rates";
            }
            auto result =
                _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
                    mavlink_message_t message;
                    mavlink_msg_gimbal_manager_set_attitude_pack_chan(
                        mavlink_address.system_id,
                        mavlink_address.component_id,
                        channel,
                        &message,
                        _system_impl->get_system_id(),
                        address.gimbal_manager_compid,
                        flags,
                        address.gimbal_device_id,
                        quaternion,
                        to_rad_from_deg(roll_rate_deg_s),
                        to_rad_from_deg(pitch_rate_deg_s),
                        to_rad_from_deg(yaw_rate_deg_s));
                    return message;
                }) ?
                    Gimbal::Result::Success :
                    Gimbal::Result::Error;
            if (callback) {
                callback(result);
            }
            break;
        }
        case Gimbal::SendMode::Once:
            if (roll_rate_deg_s != 0.0f) {
                LogWarn() << "Roll rate needs to be 0 for SendMode::Once.";
                if (callback) {
                    callback(Gimbal::Result::InvalidArgument);
                }

            } else {
                if (_debugging) {
                    LogDebug() << "Sending DO_GIMBAL_MANAGER_PITCHYAW command with angular rates";
                }

                MavlinkCommandSender::CommandInt command{};
                command.command = MAV_CMD_DO_GIMBAL_MANAGER_PITCHYAW;
                command.params.maybe_param1 = NAN;
                command.params.maybe_param2 = NAN;
                command.params.maybe_param3 = pitch_rate_deg_s;
                command.params.maybe_param4 = yaw_rate_deg_s;
                command.params.x = static_cast<int32_t>(flags);
                command.params.maybe_z = address.gimbal_device_id;
                command.target_system_id = _system_impl->get_system_id();
                command.target_component_id = address.gimbal_manager_compid;

                _system_impl->send_command_async(
                    command, [callback](MavlinkCommandSender::Result result, float) {
                        receive_command_result(result, callback);
                    });
            }
            break;
        default:
            LogErr() << "Invalid send mode";
            callback(Gimbal::Result::InvalidArgument);
            break;
    }
}

Gimbal::Result GimbalImpl::set_roi_location(
    int32_t gimbal_id, double latitude_deg, double longitude_deg, float altitude_m)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    set_roi_location_async(
        gimbal_id, latitude_deg, longitude_deg, altitude_m, [&prom](Gimbal::Result result) {
            prom.set_value(result);
        });

    return fut.get();
}

void GimbalImpl::set_roi_location_async(
    int32_t gimbal_id,
    double latitude_deg,
    double longitude_deg,
    float altitude_m,
    const Gimbal::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_address = maybe_address_for_gimbal_id(gimbal_id);

    if (!maybe_address) {
        if (callback) {
            _system_impl->call_user_callback(
                [callback]() { callback(Gimbal::Result::InvalidArgument); });
        }
        return;
    }

    auto address = maybe_address.value();

    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.maybe_param1 = address.gimbal_device_id;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_system_id = _system_impl->get_system_id();
    command.target_component_id = address.gimbal_manager_compid;

    _system_impl->send_command_async(
        command, [callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

Gimbal::Result GimbalImpl::take_control(int32_t gimbal_id, Gimbal::ControlMode control_mode)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    take_control_async(
        gimbal_id, control_mode, [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalImpl::take_control_async(
    int32_t gimbal_id, Gimbal::ControlMode control_mode, const Gimbal::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto maybe_address = maybe_address_for_gimbal_id(gimbal_id);

    if (!maybe_address) {
        if (callback) {
            callback(Gimbal::Result::InvalidArgument);
        }
        return;
    }

    auto address = maybe_address.value();

    if (control_mode == Gimbal::ControlMode::None) {
        release_control_async(gimbal_id, callback);
        return;
    } else if (control_mode == Gimbal::ControlMode::Secondary) {
        if (callback) {
            _system_impl->call_user_callback(
                [callback]() { callback(Gimbal::Result::Unsupported); });
        }
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

    command.params.maybe_param7 = address.gimbal_device_id;
    command.target_system_id = _system_impl->get_system_id();
    command.target_component_id = address.gimbal_manager_compid;

    _system_impl->send_command_async(
        command, [callback](MavlinkCommandSender::Result result, float) {
            GimbalImpl::receive_command_result(result, callback);
        });
}

Gimbal::Result GimbalImpl::release_control(int32_t gimbal_id)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    release_control_async(gimbal_id, [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalImpl::release_control_async(int32_t gimbal_id, const Gimbal::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE;
    command.params.maybe_param1 = -3.0f; // sysid primary control
    command.params.maybe_param2 = -3.0f; // compid primary control
    command.params.maybe_param3 = -3.0f; // sysid secondary control
    command.params.maybe_param4 = -3.0f; // compid secondary control

    auto maybe_address = maybe_address_for_gimbal_id(gimbal_id);

    if (!maybe_address) {
        if (callback) {
            _system_impl->call_user_callback(
                [callback]() { callback(Gimbal::Result::InvalidArgument); });
        }
        return;
    }

    auto address = maybe_address.value();

    command.target_component_id = address.gimbal_manager_compid;
    command.target_system_id = _system_impl->get_system_id();
    command.params.maybe_param7 = address.gimbal_device_id;

    _system_impl->send_command_async(
        command, [callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

std::pair<Gimbal::Result, Gimbal::ControlStatus> GimbalImpl::get_control_status(int32_t gimbal_id)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto* maybe_gimbal = maybe_gimbal_item_for_gimbal_id(gimbal_id);

    if (!maybe_gimbal) {
        return {Gimbal::Result::InvalidArgument, {}};
    }

    auto& gimbal = *maybe_gimbal;
    return {Gimbal::Result::Success, gimbal.control_status};
}

Gimbal::GimbalListHandle
GimbalImpl::subscribe_gimbal_list(const Gimbal::GimbalListCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto handle = _gimbal_list_subscriptions.subscribe(callback);

    if (std::any_of(_gimbals.begin(), _gimbals.end(), [](const GimbalItem& item) {
            return item.is_valid;
        })) {
        // We already have some gimbals detected that we need to tell the subscriber.
        _gimbal_list_subscriptions.queue(gimbal_list_with_lock(), [this](const auto& func) {
            _system_impl->call_user_callback(func);
        });
    }

    return handle;
}

void GimbalImpl::unsubscribe_gimbal_list(Gimbal::GimbalListHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _gimbal_list_subscriptions.unsubscribe(handle);
}

Gimbal::GimbalList GimbalImpl::gimbal_list()
{
    std::lock_guard<std::mutex> lock(_mutex);

    return gimbal_list_with_lock();
}

Gimbal::GimbalList GimbalImpl::gimbal_list_with_lock()
{
    // Obviously assume lock

    int32_t gimbal_id = 1;
    Gimbal::GimbalList new_list;
    for (auto& gimbal : _gimbals) {
        if (!gimbal.is_valid) {
            continue;
        }
        Gimbal::GimbalItem new_item{};
        new_item.gimbal_id = gimbal_id++;
        new_item.gimbal_device_id = gimbal.gimbal_device_id;
        new_item.gimbal_manager_component_id = gimbal.gimbal_manager_compid;
        new_item.vendor_name = gimbal.vendor_name;
        new_item.model_name = gimbal.model_name;
        new_item.custom_name = gimbal.custom_name;
        new_list.gimbals.emplace_back(std::move(new_item));
    }
    return new_list;
}

Gimbal::ControlStatusHandle
GimbalImpl::subscribe_control_status(const Gimbal::ControlStatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _control_status_subscriptions.subscribe(callback);
}

void GimbalImpl::unsubscribe_control_status(Gimbal::ControlStatusHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _control_status_subscriptions.unsubscribe(handle);
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

std::pair<Gimbal::Result, Gimbal::Attitude> GimbalImpl::get_attitude(int32_t gimbal_id)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto* maybe_gimbal = maybe_gimbal_item_for_gimbal_id(gimbal_id);

    if (!maybe_gimbal) {
        return {Gimbal::Result::InvalidArgument, {}};
    }

    auto& gimbal = *maybe_gimbal;

    return {Gimbal::Result::Success, gimbal.attitude};
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

std::optional<GimbalImpl::GimbalAddress>
GimbalImpl::maybe_address_for_gimbal_id(int32_t gimbal_id) const
{
    // Assumes lock

    if (gimbal_id < 0) {
        LogWarn() << "Invalid negative gimbal ID: " << gimbal_id;
        return {};
    }

    if (gimbal_id > _gimbals.size()) {
        LogWarn() << "Invalid positive gimbal ID: " << gimbal_id;
        return {};
    }

    if (gimbal_id == 0) {
        return GimbalAddress{0, 0};
    }

    return GimbalAddress{
        _gimbals[gimbal_id - 1].gimbal_manager_compid, _gimbals[gimbal_id - 1].gimbal_device_id};
}

GimbalImpl::GimbalItem* GimbalImpl::maybe_gimbal_item_for_gimbal_id(int32_t gimbal_id)
{
    // Assumes lock

    if (gimbal_id == 0) {
        LogWarn() << "Invalid gimbal ID: " << gimbal_id;
        return nullptr;
    }

    if (gimbal_id < 0) {
        LogWarn() << "Invalid negative gimbal ID: " << gimbal_id;
        return nullptr;
    }

    if (gimbal_id > _gimbals.size()) {
        LogWarn() << "Invalid positive gimbal ID: " << gimbal_id;
        return nullptr;
    }

    return &_gimbals[gimbal_id - 1];
}

} // namespace mavsdk
