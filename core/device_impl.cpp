#include "device_impl.h"
#include "global_include.h"
#include "dronecore_impl.h"
#include "mavlink_include.h"
#include <functional>

namespace dronecore {

using namespace std::placeholders; // for `_1`
std::mutex DeviceImpl::_last_heartbeat_reiceved_time_mutex;
dl_time_t DeviceImpl::_last_heartbeat_received_time;

DeviceImpl::DeviceImpl(DroneCoreImpl *parent,
                       uint8_t target_system_id) :
    _target_system_id(target_system_id),
    _parent(parent),
    _params(this),
    _commands(this)
{
    _device_thread = new std::thread(device_thread, this);

    register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        std::bind(&DeviceImpl::process_heartbeat, this, _1), this);

    register_mavlink_message_handler(
        MAVLINK_MSG_ID_AUTOPILOT_VERSION,
        std::bind(&DeviceImpl::process_autopilot_version, this, _1), this);
}

DeviceImpl::~DeviceImpl()
{
    _should_exit = true;
    unregister_all_mavlink_message_handlers(this);

    if (_device_thread != nullptr) {
        _device_thread->join();
        delete _device_thread;
        _device_thread = nullptr;
    }
}

void DeviceImpl::register_mavlink_message_handler(uint16_t msg_id,
                                                  mavlink_message_handler_t callback,
                                                  const void *cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_handler_table_mutex);

    MavlinkHandlerTableEntry entry = {msg_id, callback, cookie};
    _mavlink_handler_table.push_back(entry);
}

void DeviceImpl::unregister_all_mavlink_message_handlers(const void *cookie)
{
    std::lock_guard<std::mutex> lock(_mavlink_handler_table_mutex);

    for (auto it = _mavlink_handler_table.begin();
         it != _mavlink_handler_table.end();
         /* no ++it */) {

        if (it->cookie == cookie) {
            it = _mavlink_handler_table.erase(it);
        } else {
            ++it;
        }
    }
    _mavlink_handler_table.clear();
}

void DeviceImpl::register_timeout_handler(std::function<void()> callback,
                                          double duration_s,
                                          void **cookie)
{
    _timeout_handler.add(callback, duration_s, cookie);
}

void DeviceImpl::refresh_timeout_handler(const void *cookie)
{
    _timeout_handler.refresh(cookie);
}

void DeviceImpl::update_timeout_handler(double new_duration_s, const void *cookie)
{
    _timeout_handler.update(new_duration_s, cookie);
}

void DeviceImpl::unregister_timeout_handler(const void *cookie)
{
    _timeout_handler.remove(cookie);
}

void DeviceImpl::process_mavlink_message(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_mavlink_handler_table_mutex);

    for (auto it = _mavlink_handler_table.begin(); it != _mavlink_handler_table.end(); ++it) {
        if (it->msg_id == message.msgid) {
            it->callback(message);
        }
    }
}

void DeviceImpl::process_heartbeat(const mavlink_message_t &message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    /* If we don't know the UUID yet, or we had a timeout, we want to check that the
     * UUID is still the same. */
    if (_target_uuid == 0 || !_heartbeats_arriving) {
        request_autopilot_version();
    }

    _armed = ((heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false);

    _heartbeats_arriving = true;

    std::lock_guard<std::mutex> lock(_last_heartbeat_reiceved_time_mutex);
    _last_heartbeat_received_time = steady_time();
}

void DeviceImpl::process_autopilot_version(const mavlink_message_t &message)
{
    // Ignore if they don't come from the autopilot component
    if (message.compid != MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT) {
        return;
    }

    mavlink_autopilot_version_t autopilot_version;
    mavlink_msg_autopilot_version_decode(&message, &autopilot_version);

    _target_supports_mission_int =
        ((autopilot_version.capabilities & MAV_PROTOCOL_CAPABILITY_MISSION_INT) ? true : false);

    if (_target_uuid == 0) {
        _target_uuid = autopilot_version.uid;
        _parent->notify_on_discover(_target_uuid);

    } else if (_target_uuid == autopilot_version.uid) {
        if (!_heartbeats_arriving) {
            // It looks like the vehicle has reconnected, let's accept it again.
            _parent->notify_on_discover(_target_uuid);
        } else {
            // This means we just got a autopilot version message but we don't need it
            // or didn't request it.
        }

    } else {
        // TODO: this is bad, we should raise a flag to invalidate device.
        LogErr() << "Error: UUID changed";
    }
}

void DeviceImpl::device_thread(DeviceImpl *self)
{
    dl_time_t last_time {};

    while (!self->_should_exit) {

        if (elapsed_since_s(last_time) >= DeviceImpl::_HEARTBEAT_SEND_INTERVAL_S) {
            send_heartbeat(self);
            last_time = steady_time();
        }
        check_timeouts(self);
        check_heartbeat_timeout(self);
        self->_params.do_work();
        self->_commands.do_work();
        if (self->_heartbeats_arriving) {
            // Work fairly fast if we're connected.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            // Be less aggressive when unconnected.
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

void DeviceImpl::send_heartbeat(DeviceImpl *self)
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(_own_system_id, _own_component_id, &message,
                               MAV_TYPE_GCS, MAV_AUTOPILOT_GENERIC, 0, 0, 0);
    self->send_message(message);
}

void DeviceImpl::check_timeouts(DeviceImpl *self)
{
    self->_timeout_handler.run_once();
}

void DeviceImpl::check_heartbeat_timeout(DeviceImpl *self)
{
    std::lock_guard<std::mutex> lock(_last_heartbeat_reiceved_time_mutex);
    if (elapsed_since_s(self->_last_heartbeat_received_time) > DeviceImpl::_HEARTBEAT_TIMEOUT_S) {
        if (self->_heartbeats_arriving) {
            self->_parent->notify_on_timeout(self->_target_uuid);
            self->_heartbeats_arriving = false;
        }
    } else {
        if (!self->_heartbeats_arriving) {
            self->_heartbeats_arriving = true;
        }
    }
}

bool DeviceImpl::send_message(const mavlink_message_t &message)
{
    return _parent->send_message(message);
}

void DeviceImpl::request_autopilot_version()
{
    send_command_with_ack_async(
        MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES,
        MavlinkCommands::Params {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
        nullptr,
        MavlinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

uint64_t DeviceImpl::get_target_uuid() const
{
    return _target_uuid;
}

uint8_t DeviceImpl::get_target_system_id() const
{
    return _target_system_id;
}

uint8_t DeviceImpl::get_target_component_id() const
{
    return _target_component_id;
}

void DeviceImpl::set_target_system_id(uint8_t system_id)
{
    _target_system_id = system_id;
}

void DeviceImpl::set_param_float_async(const std::string &name, float value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_float(value);
    _params.set_param_async(name, param_value, callback);
}

void DeviceImpl::set_param_int_async(const std::string &name, int32_t value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_int(value);
    _params.set_param_async(name, param_value, callback);
}

void DeviceImpl::set_param_ext_float_async(const std::string &name, float value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_float(value);
    _params.set_param_async(name, param_value, callback, true);
}

void DeviceImpl::set_param_ext_int_async(const std::string &name, int32_t value, success_t callback)
{
    MavlinkParameters::ParamValue param_value;
    param_value.set_int(value);
    _params.set_param_async(name, param_value, callback, true);
}

void DeviceImpl::get_param_float_async(const std::string &name,
                                       get_param_float_callback_t callback)
{
    _params.get_param_async(name, std::bind(&DeviceImpl::receive_float_param, _1, _2,
                                            callback));
}

void DeviceImpl::get_param_int_async(const std::string &name,
                                     get_param_int_callback_t callback)
{
    _params.get_param_async(name, std::bind(&DeviceImpl::receive_int_param, _1, _2,
                                            callback));
}

void DeviceImpl::get_param_ext_float_async(const std::string &name,
                                           get_param_float_callback_t callback)
{
    _params.get_param_async(name, std::bind(&DeviceImpl::receive_float_param, _1, _2,
                                            callback), true);
}

void DeviceImpl::get_param_ext_int_async(const std::string &name,
                                         get_param_int_callback_t callback)
{
    _params.get_param_async(name, std::bind(&DeviceImpl::receive_int_param, _1, _2,
                                            callback), true);
}

void DeviceImpl::receive_float_param(bool success, MavlinkParameters::ParamValue value,
                                     get_param_float_callback_t callback)
{
    if (callback) {
        callback(success, value.get_float());
    }
}

void DeviceImpl::receive_int_param(bool success, MavlinkParameters::ParamValue value,
                                   get_param_int_callback_t callback)
{
    if (callback) {
        callback(success, value.get_int());
    }
}

MavlinkCommands::Result DeviceImpl::send_command_with_ack(
    uint16_t command, const MavlinkCommands::Params &params, uint8_t component_id)
{
    if (_target_system_id == 0 && _target_component_id == 0) {
        return MavlinkCommands::Result::NO_DEVICE;
    }

    const uint8_t component_id_to_use =
        ((component_id != 0) ? component_id : _target_component_id);

    return _commands.send_command(command, params, _target_system_id, component_id_to_use);
}

void DeviceImpl::send_command_with_ack_async(uint16_t command,
                                             const MavlinkCommands::Params &params,
                                             command_result_callback_t callback,
                                             uint8_t component_id)
{
    if (_target_system_id == 0 && _target_component_id == 0) {
        if (callback) {
            callback(MavlinkCommands::Result::NO_DEVICE, NAN);
        }
        return;
    }

    const uint8_t component_id_to_use =
        ((component_id != 0) ? component_id : _target_component_id);

    _commands.queue_command_async(command, params, _target_system_id, component_id_to_use,
                                  callback);
}

MavlinkCommands::Result DeviceImpl::set_msg_rate(uint16_t message_id, double rate_hz)
{
    // If left at -1 it will stop the message stream.
    float interval_us = -1.0f;
    if (rate_hz > 0) {
        interval_us = 1e6f / (float)rate_hz;
    }

    return send_command_with_ack(
               MAV_CMD_SET_MESSAGE_INTERVAL,
               MavlinkCommands::Params {float(message_id), interval_us, NAN, NAN, NAN, NAN, NAN});
}

void DeviceImpl::set_msg_rate_async(uint16_t message_id, double rate_hz,
                                    command_result_callback_t callback)
{
    // If left at -1 it will stop the message stream.
    float interval_us = -1.0f;
    if (rate_hz > 0) {
        interval_us = 1e6f / (float)rate_hz;
    }

    send_command_with_ack_async(
        MAV_CMD_SET_MESSAGE_INTERVAL,
        MavlinkCommands::Params {float(message_id), interval_us, NAN, NAN, NAN, NAN, NAN},
        callback);
}


} // namespace dronecore
