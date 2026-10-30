#pragma once

#include "autopilot.hpp"
#include "mavlink_include.hpp"
#include "mavsdk_export.h"
#include "plugins/modes/modes.hpp"

#include <string>
#include <vector>

namespace mavsdk {

// A mode together with its MAVLink standard mode (MAV_STANDARD_MODE), which we need to set it
// but don't expose.
struct ModeEntry {
    Modes::Mode mode;
    uint8_t standard_mode;
};

// Name used for a standard mode. Vehicles don't send names for standard modes, so we use the
// same names as QGroundControl. Empty for non-standard or unknown standard modes.
MAVSDK_TEST_EXPORT std::string modes_standard_mode_name(uint8_t standard_mode);

// Known modes for autopilots that don't implement AVAILABLE_MODES.
MAVSDK_TEST_EXPORT std::vector<ModeEntry>
modes_fallback(Autopilot autopilot, MAV_TYPE vehicle_type);

} // namespace mavsdk
