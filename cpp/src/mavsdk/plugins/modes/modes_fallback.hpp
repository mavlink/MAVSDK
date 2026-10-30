#pragma once

#include "autopilot.hpp"
#include "mavlink_include.hpp"
#include "mavsdk_export.h"
#include "plugins/modes/modes.hpp"
#include "standard_modes.hpp"

#include <string>
#include <vector>

namespace mavsdk {

// A mode together with its MAVLink standard mode (MAV_STANDARD_MODE), which we need to set it
// but don't expose.
struct ModeEntry {
    Modes::Mode mode;
    uint8_t standard_mode;
};

// Known modes for autopilots that don't implement AVAILABLE_MODES.
MAVSDK_TEST_EXPORT std::vector<ModeEntry>
modes_fallback(Autopilot autopilot, MAV_TYPE vehicle_type);

} // namespace mavsdk
