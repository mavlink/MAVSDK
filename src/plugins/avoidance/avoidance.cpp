#include "plugins/avoidance/avoidance.h"
#include "avoidance_impl.h"
#include <vector>

namespace mavsdk {

Avoidance::Avoidance(System& system) : PluginBase(), _impl{new AvoidanceImpl(system)} {}

Avoidance::~Avoidance() {}

void Avoidance::receive_trajectory_representation_waypoints_async(
    trajectory_representation_waypoints_callback_t callback)
{
    return _impl->receive_trajectory_representation_waypoints_async(callback);
}

const char* Avoidance::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        default:
            return "Unknown";
    }
}

} // namespace mavsdk
