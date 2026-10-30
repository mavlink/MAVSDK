#pragma once

#include "autopilot.hpp"
#include "mavlink_include.hpp"
#include "mavsdk_export.h"
#include "plugins/modes/modes.hpp"

#include <string>
#include <vector>

namespace mavsdk {

// Name used for a standard mode. Vehicles don't send names for standard modes, so we use the
// same names as QGroundControl.
MAVSDK_TEST_EXPORT std::string modes_standard_mode_name(Modes::StandardMode standard_mode);

// Known modes for autopilots that don't implement AVAILABLE_MODES.
MAVSDK_TEST_EXPORT std::vector<Modes::Mode> modes_fallback(Autopilot autopilot, MAV_TYPE vehicle_type);

} // namespace mavsdk
