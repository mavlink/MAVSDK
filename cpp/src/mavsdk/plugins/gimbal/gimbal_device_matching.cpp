#include "gimbal_device_matching.hpp"

namespace mavsdk {

bool gimbal_device_message_matches(
    uint8_t manager_compid,
    uint8_t advertised_gimbal_device_id,
    uint8_t device_compid,
    uint8_t device_msg_gimbal_device_id)
{
    if (device_msg_gimbal_device_id == 0) {
        // Separate gimbal device component: identified by its own compid, which the
        // manager advertised as its gimbal_device_id.
        return advertised_gimbal_device_id == device_compid;
    }
    // Non-MAVLink gimbal device (id 1-6): it shares the manager's component.
    return manager_compid == device_compid &&
           advertised_gimbal_device_id == device_msg_gimbal_device_id;
}

} // namespace mavsdk
