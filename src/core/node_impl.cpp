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
    _parent(parent)
{
    node_address.system_id = system_id;
    node_address.component_id = comp_id;
}

NodeImpl::~NodeImpl()
{
    _should_exit = true;
}

void NodeImpl::process_mavlink_message(mavlink_message_t& message)
{
    if (false) LogDebug() << "Received message: " << int(message.msgid);
    return;
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

} // namespace mavsdk
