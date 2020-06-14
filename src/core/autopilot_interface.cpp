#include "node_impl.h"
#include "autopilot_interface.h"
#include "autopilot_interface_impl.h"

namespace mavsdk {

using namespace std::placeholders; // for `_1`

AutopilotInterface::AutopilotInterface(Node& parent) :
    _parent(parent),
    _impl{new AutopilotInterfaceImpl(parent.node_impl())}
{
}

uint64_t AutopilotInterface::get_uuid() const
{
    return impl()->get_uuid();
}

} // namespace mavsdk
