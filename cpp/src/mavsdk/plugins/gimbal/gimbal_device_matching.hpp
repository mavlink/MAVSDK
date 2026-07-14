#pragma once

#include <cstdint>

namespace mavsdk {

// Correlate a GIMBAL_DEVICE_* message (GIMBAL_DEVICE_INFORMATION or
// GIMBAL_DEVICE_ATTITUDE_STATUS) back to the gimbal a manager advertised.
//
// A gimbal manager announces which device it is responsible for via
// GIMBAL_MANAGER_INFORMATION.gimbal_device_id (here: advertised_gimbal_device_id):
//   - 1-6:   a non-MAVLink gimbal device that shares the manager's component, so the
//            device's messages come from the manager's component ID and carry the same
//            1-6 id.
//   - 7-255: a gimbal device that is its own MAVLink component; its messages come from
//            that component ID and carry a gimbal_device_id of 0 (device_msg_gimbal_device_id).
//
// Returns whether the gimbal identified by (manager_compid, advertised_gimbal_device_id)
// is the one that sent a device message from device_compid carrying
// device_msg_gimbal_device_id.
bool gimbal_device_message_matches(
    uint8_t manager_compid,
    uint8_t advertised_gimbal_device_id,
    uint8_t device_compid,
    uint8_t device_msg_gimbal_device_id);

} // namespace mavsdk
