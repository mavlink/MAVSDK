#include "../node_impl.h"
#include "autopilot.h"

namespace mavsdk {

void NodeImpl::get_autopilot_info_async(const Autopilot::autopilot_info_callback_t callback) {
    (void) callback;
}

bool NodeImpl::has_capability_autopilot() const {
    return false;
}

void NodeImpl::set_flight_mode_async(Autopilot::FlightMode mode,
        NodeImpl::command_result_callback_t callback) {
    (void) mode;
    (void) callback;
}

} // namespace mavsdk
