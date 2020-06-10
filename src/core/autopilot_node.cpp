#include "autopilot_node.h"
#include "autopilot_node_impl.h"

namespace mavsdk {

using namespace std::placeholders; // for `_1`

uint64_t AutopilotNode::get_uuid() const
{
    return autopilot_node_impl()->get_uuid();
}

} // namespace mavsdk
