#include "node.h"
#include "global_include.h"
#include "mavsdk_impl.h"
#include "mavlink_include.h"
#include "node_impl.h"
#include "plugin_impl_base.h"
#include <functional>
#include <algorithm>
#include "px4_custom_mode.h"

// Set to 1 to log incoming/outgoing mavlink messages.
#define MESSAGE_DEBUGGING 1

namespace mavsdk {

using namespace std::placeholders; // for `_1`

Node::Node(MavsdkImpl& parent, uint8_t system_id, uint8_t component_id) :
    _node_impl(std::make_shared<NodeImpl>(parent, system_id, component_id))
{}

Node::~Node() {}

bool Node::is_connected() const
{
    return _node_impl->is_connected();
}

uint8_t Node::get_system_id() const
{
    return _node_impl->get_system_id();
}

uint8_t Node::get_component_id() const
{
    return _node_impl->get_component_id();
}

} // namespace mavsdk
