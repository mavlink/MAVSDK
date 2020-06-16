#include "node.h"
#include "global_include.h"
#include "mavsdk_impl.h"
#include "mavlink_include.h"
#include "mavlink_parameters.h"
#include "node_impl.h"
#include "plugin_impl_base.h"
#include "px4_custom_mode.h"
#include <functional>
#include <algorithm>
#include <future>

// Set to 1 to log incoming.outgoing mavlink messages.
#define MESSAGE_DEBUGGING 0

namespace mavsdk {

using namespace std::placeholders; // for `_1`

NodeImpl::NodeImpl(MavsdkImpl& parent, uint8_t system_id, uint8_t comp_id) :
    Sender(parent.own_address, node_address),
    _parent(parent),
    _params(*this),
    _commands(*this),
    _timesync(*this),
    _timeout_handler(_time),
    _call_every_handler(_time),
    _heartbeat(),
    _interfaces()
{
    node_address.system_id = system_id;
    node_address.component_id = comp_id;

    _system_thread = new std::thread(&NodeImpl::system_thread, this);

    std::cout << "Here"<< std::endl;
    _message_handler.register_one(
            MAVLINK_MSG_ID_HEARTBEAT, std::bind(&NodeImpl::process_heartbeat, this, _1), this);
}

NodeImpl::~NodeImpl()
{
    _should_exit = true;
}

void NodeImpl::process_mavlink_message(mavlink_message_t& message)
{
    _message_handler.process_message(message);
}

bool NodeImpl::send_message(mavlink_message_t& message)
{
    // TODO intercept callback

    LogDebug()<< "Sending message: " << size_t(message.msgid);

    return _parent.send_message(message);
}

uint8_t NodeImpl::get_system_id()
{
    return node_address.system_id;
}

uint8_t NodeImpl::get_component_id()
{
    return node_address.component_id;
}

uint8_t NodeImpl::get_own_system_id()
{
    return _parent.get_own_system_id();
}

uint8_t NodeImpl::get_own_component_id()
{
    return _parent.get_own_component_id();
}

bool NodeImpl::is_connected()
{
    // TODO implement heartbeat timeout 
    return true;
}

void NodeImpl::process_heartbeat(const mavlink_message_t& message)
{
    mavlink_msg_heartbeat_decode(&message, &_heartbeat);

    refresh_timeout_handler(_heartbeat_timeout_cookie);
}

void NodeImpl::register_timeout_handler(
    std::function<void()> callback, double duration_s, void** cookie)
{
    _timeout_handler.add(callback, duration_s, cookie);
}

void NodeImpl::refresh_timeout_handler(const void* cookie)
{
    _timeout_handler.refresh(cookie);
}

void NodeImpl::unregister_timeout_handler(const void* cookie)
{
     _timeout_handler.remove(cookie);
}

uint8_t NodeImpl::get_type() const
{
    return _heartbeat.type;
}

uint8_t NodeImpl::get_autopilot() const
{
    return _heartbeat.autopilot;
}

uint8_t NodeImpl::get_base_mode() const
{
    return _heartbeat.base_mode;
}

uint8_t NodeImpl::get_system_status() const
{
    return _heartbeat.system_status;
}

// TODO: certain is_xyz functions will take time,
// should they be blocking or async?
bool NodeImpl::is_autopilot() const
{
    return get_autopilot() != MAV_AUTOPILOT_INVALID;
}

MAVLinkCommands::Result NodeImpl::send_command(MAVLinkCommands::CommandLong& command)
{
    if (target_address.system_id == 0 && target_address.component_id == 0) {
        // TODO: should rename to NoNode or something
        return MAVLinkCommands::Result::NoSystem;
    }
    command.target_system_id = get_system_id();
    return _commands.send_command(command);
}

MAVLinkCommands::Result NodeImpl::send_command(MAVLinkCommands::CommandInt& command)
{
    if (target_address.system_id == 0 && target_address.component_id == 0) {
        // TODO: should rename to NoNode or something
        return MAVLinkCommands::Result::NoSystem;
    }
    command.target_system_id = get_system_id();
    return _commands.send_command(command);
}

void NodeImpl::send_command_async(
        MAVLinkCommands::CommandLong& command, const commandResultCallback callback)
{
    if (target_address.system_id == 0 && target_address.component_id == 0) {
        // TODO: should rename to NoNode or something
        if (callback) {
            callback(MAVLinkCommands::Result::NoSystem, NAN);
        }
    }
    command.target_system_id = get_system_id();
    command.target_component_id = get_component_id();

    _commands.queue_command_async(command, callback);
}

void NodeImpl::send_command_async(
        MAVLinkCommands::CommandInt& command, const commandResultCallback callback)
{
    if (target_address.system_id == 0 && target_address.component_id == 0) {
        // TODO: should rename to NoNode or something
        if (callback) {
            callback(MAVLinkCommands::Result::NoSystem, NAN);
        }
    }
    command.target_system_id = get_system_id();

    _commands.queue_command_async(command, callback);
}

void NodeImpl::call_user_callback(const std::function<void()>& func)
{
    _thread_pool.enqueue(func);
}

void NodeImpl::register_mavlink_message_handler(
        uint16_t msg_id, mavlink_message_handler_t callback, const void* cookie)
{
    _message_handler.register_one(msg_id, callback, cookie);
}

void NodeImpl::unregister_mavlink_message_handler(
        uint16_t msg_id, const void* cookie)
{
    _message_handler.unregister_one(msg_id, cookie);
}

void NodeImpl::unregister_all_mavlink_message_handlers(const void* cookie)
{
    _message_handler.unregister_all(cookie);
}

void NodeImpl::system_thread()
{
    dl_time_t last_time{};

    while (!_should_exit) {
         _call_every_handler.run_once();
        _timeout_handler.run_once();
        _params.do_work();
        _commands.do_work();
        _timesync.do_work();
        // _mission_transfer.do_work();
        
        if (is_connected()) {
            // Work fairly fast if we're connected
             std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            // Be less aggressive when unconnected.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

// TODO get_own_mav_type()
// TODO autopilot time and stuff
// TODO heartbeats_timed_out
// TODO mission transfer
// TODO connected or not

void NodeImpl::register_plugin(PluginImplBase* plugin_impl)
{
    assert(plugin_impl);

    plugin_impl->init();

    {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        _plugin_impls.push_back(plugin_impl);
    }

    // If we're connected already, let's enable it straightaway
    // TODO figure out connected logic
    if (is_connected()) {
        plugin_impl->enable();
    }
}

void NodeImpl::unregister_plugin(PluginImplBase* plugin_impl)
{
    assert(plugin_impl);

    plugin_impl->disable();
    plugin_impl->deinit();

    // Remove first, so it won't get enabled/dsiable anymore
    {
        std::lock_guard<std::mutex> lock(_plugin_impls_mutex);
        auto found = std::find(_plugin_impls.begin(), _plugin_impls.end(), plugin_impl);
        if (found != _plugin_impls.end()) {
            _plugin_impls.erase(found);
        }
    }
}

void NodeImpl::add_call_every(std::function<void()> callback, float interval_s, void** cookie)
{
    _call_every_handler.add(callback, interval_s, cookie);
}
void NodeImpl::change_call_every(float interval_s, const void* cookie)
{
    _call_every_handler.change(interval_s, cookie);
}
void NodeImpl::reset_call_every(const void* cookie)
{
    _call_every_handler.reset(cookie);
}
void NodeImpl::remove_call_every(const void* cookie)
{
    _call_every_handler.remove(cookie);
}

MAVLinkCommands::Result NodeImpl::set_msg_rate(uint16_t message_id, double rate_hz)
{
    MAVLinkCommands::CommandLong command = make_command_msg_rate(message_id, rate_hz);
    return send_command(command);
}

void NodeImpl::set_msg_rate_async(
        uint16_t message_id, double rate_hz, commandResultCallback callback)
{
    MAVLinkCommands::CommandLong command = make_command_msg_rate(message_id, rate_hz);
    send_command_async(command, callback);
}

MAVLinkCommands::CommandLong NodeImpl::make_command_msg_rate(uint16_t message_id, double rate_hz)
{
    MAVLinkCommands::CommandLong command{};

    // 0 to request default rate, -1 to stop stream

    float interval_us = 0.0f;

    if (rate_hz > 0) {
        interval_us = 1e6f / static_cast<float>(rate_hz);
    } else if (rate_hz < 0) {
        interval_us = -1.0f;
    }

    command.command = MAV_CMD_SET_MESSAGE_INTERVAL;
    command.params.param1 = float(message_id);
    command.params.param2 = interval_us;

    return command;
}

MAVLinkParameters::Result NodeImpl::set_param_float(const std::string& name, float value)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_float(value);

    return _params.set_param(name, param_value, false);
}

MAVLinkParameters::Result NodeImpl::set_param_int(const std::string& name, int32_t value)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_int32(value);

    return _params.set_param(name, param_value, false);
}
MAVLinkParameters::Result NodeImpl::set_param_ext_float(const std::string& name, float value)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_float(value);

    return _params.set_param(name, param_value, true);
}
MAVLinkParameters::Result NodeImpl::set_param_ext_int(const std::string& name, int32_t value)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_int32(value);

    return _params.set_param(name, param_value, true);
}

void NodeImpl::set_param_float_async(
        const std::string& name, float value, success_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_float(value);
    _params.set_param_async(name, param_value, callback, cookie);
}

void NodeImpl::set_param_int_async(
        const std::string& name, int32_t value, success_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_int32(value);
    _params.set_param_async(name, param_value, callback, cookie);
}
void NodeImpl::set_param_ext_float_async(
        const std::string& name, float value, success_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_float(value);
    _params.set_param_async(name, param_value, callback, cookie, true);
}
void NodeImpl::set_param_ext_int_async(
        const std::string& name, int32_t value, success_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue param_value;
    param_value.set_int32(value);
    _params.set_param_async(name, param_value, callback, cookie, true);
}

std::pair<MAVLinkParameters::Result, float> NodeImpl::get_param_float(const std::string& name)
{
    auto prom = std::promise<std::pair<MAVLinkParameters::Result, float>>();
    auto res = prom.get_future();

    MAVLinkParameters::ParamValue value_type;
    value_type.set_float(0.0f);

    _params.get_param_async(
        name,value_type,
        [&prom](MAVLinkParameters::Result result, MAVLinkParameters::ParamValue param) {
            float value = NAN;
            if (result == MAVLinkParameters::Result::Success) {
                value = param.get_float();
            }
            prom.set_value(std::make_pair<>(result, value));
        },
        this);

    return res.get();
}

std::pair<MAVLinkParameters::Result, int> NodeImpl::get_param_int(const std::string& name)
{
    auto prom = std::promise<std::pair<MAVLinkParameters::Result, int>>();
    auto res = prom.get_future();

    MAVLinkParameters::ParamValue value_type;
    value_type.set_int32(0);

    _params.get_param_async(
        name,
        value_type,
        [&prom](MAVLinkParameters::Result result, MAVLinkParameters::ParamValue param) {
            int value = 0;
            if (result == MAVLinkParameters::Result::Success) {
                value = param.get_int32();
            }
            prom.set_value(std::make_pair<>(result, value));
        },
        this);

    return res.get();
}

std::pair<MAVLinkParameters::Result, float> NodeImpl::get_param_ext_float(const std::string& name)
{
    auto prom = std::promise<std::pair<MAVLinkParameters::Result, float>>();
    auto res = prom.get_future();

    MAVLinkParameters::ParamValue value_type;
    value_type.set_float(0.0f);

    _params.get_param_async(
        name,
        value_type,
        [&prom](MAVLinkParameters::Result result, MAVLinkParameters::ParamValue param) {
            float value = NAN;
            if (result == MAVLinkParameters::Result::Success) {
                value = param.get_float();
            }
            prom.set_value(std::make_pair<>(result, value));
        },
        this,
        true);

    return res.get();
}

std::pair<MAVLinkParameters::Result, int> NodeImpl::get_param_ext_int(const std::string& name)
{
    auto prom = std::promise<std::pair<MAVLinkParameters::Result, int>>();
    auto res = prom.get_future();

    MAVLinkParameters::ParamValue value_type;
    value_type.set_int32(0);

    _params.get_param_async(
        name,
        value_type,
        [&prom](MAVLinkParameters::Result result, MAVLinkParameters::ParamValue param) {
            int value = 0;
            if (result == MAVLinkParameters::Result::Success) {
                value = param.get_int32();
            }
            prom.set_value(std::make_pair<>(result, value));
        },
        this,
        true);

    return res.get();
}

void NodeImpl::get_param_float_async(
    const std::string& name, get_param_float_callback_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue value_type;
    value_type.set_float(0.0f);

    _params.get_param_async(
        name, value_type, std::bind(&NodeImpl::receive_float_param, _1, _2, callback), cookie);
}

void NodeImpl::get_param_int_async(
    const std::string& name, get_param_int_callback_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue value_type;
    value_type.set_int32(0);

    _params.get_param_async(
        name, value_type, std::bind(&NodeImpl::receive_int_param, _1, _2, callback), cookie);
}

void NodeImpl::get_param_ext_float_async(
    const std::string& name, get_param_float_callback_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue value_type;
    value_type.set_float(0.0f);

    _params.get_param_async(
        name,
        value_type,
        std::bind(&NodeImpl::receive_float_param, _1, _2, callback),
        cookie,
        true);
}

void NodeImpl::get_param_ext_int_async(
    const std::string& name, get_param_int_callback_t callback, const void* cookie)
{
    MAVLinkParameters::ParamValue value_type;
    value_type.set_int32(0);

    _params.get_param_async(
        name,
        value_type,
        std::bind(&NodeImpl::receive_int_param, _1, _2, callback),
        cookie,
        true);
}

void NodeImpl::receive_float_param(
    MAVLinkParameters::Result result,
    MAVLinkParameters::ParamValue value,
    get_param_float_callback_t callback)
{
    if (callback) {
        if (result == MAVLinkParameters::Result::Success) {
            callback(result, value.get_float());
        } else {
            callback(result, NAN);
        }
    }
}

void NodeImpl::receive_int_param(
    MAVLinkParameters::Result result,
    MAVLinkParameters::ParamValue value,
    get_param_int_callback_t callback)
{
    if (callback) {
        if (result == MAVLinkParameters::Result::Success) {
            callback(result, value.get_int32());
        } else {
            callback(result, 0);
        }
    }
}

} // namespace mavsdk
