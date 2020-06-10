#include "autopilot_node_impl.h"
#include "node.h"

namespace mavsdk {

uint64_t AutopilotNodeImpl::get_uuid() const
{
    return 1234;
};

bool AutopilotNodeImpl::is_autopilot(Node& node)
{
    return node.node_impl()->get_autopilot() != MAV_AUTOPILOT_INVALID;
}

} // namespace mavsdk
