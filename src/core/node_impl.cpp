#include "node.h"
#include "global_include.h"
#include "mavsdk_impl.h"
#include "mavlink_include.h"
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
    _commands(*this),
    _timesync(*this),
    _timeout_handler(_time),
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
    return false;
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
        MAVLinkCommands::CommandLong& command, const command_result_callback_t callback)
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
        MAVLinkCommands::CommandInt& command, const command_result_callback_t callback)
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
        // _call_every_handler.run_once();
        _timeout_handler.run_once();
        //_params.do_work();
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

// TODO *_call_every
// TODO set_msg_rate(_async)
// TODO get_own_mav_type()
// TODO autopilot time and stuff
// TODO register plugins
// TODO heartbeats_timed_out
// TODO mission transfer
// TODO make_command_msg_rate

} // namespace mavsdk
