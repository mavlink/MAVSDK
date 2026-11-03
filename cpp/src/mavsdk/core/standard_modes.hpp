#pragma once

#include "mavsdk_export.h"

#include <cstdint>
#include <string>

namespace mavsdk {

// Name used for a MAVLink standard mode (MAV_STANDARD_MODE). Vehicles don't send names for
// standard modes, so we use the same names as QGroundControl. Empty for non-standard or unknown
// standard modes.
MAVSDK_TEST_EXPORT std::string standard_mode_name(uint8_t standard_mode);

// The reverse, MAV_STANDARD_MODE_NON_STANDARD for unknown names.
MAVSDK_TEST_EXPORT uint8_t standard_mode_from_name(const std::string& name);

} // namespace mavsdk
