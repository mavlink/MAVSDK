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
    _timeout_handler(_time),
    _heartbeat()
{
    node_address.system_id = system_id;
    node_address.component_id = comp_id;

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
    LogDebug()<< "Sending message: " << int(message.msgid);

    return true;
}

uint8_t NodeImpl::get_system_id()
{
    return node_address.system_id;
}

uint8_t NodeImpl::get_component_id()
{
    return node_address.component_id;
}

bool NodeImpl::is_connected()
{
    return false;
}

void NodeImpl::process_heartbeat(const mavlink_message_t& message)
{
    std::cout<< "Heartbeat" << std::endl;
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

} // namespace mavsdk
