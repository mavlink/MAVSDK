#pragma once

#include <sstream>

namespace mavsdk {

/**
 * @brief Compatibility mode for MAVLink protocol behavior.
 *
 * This setting determines which autopilot-specific quirks and behaviors
 * are used when communicating via MAVLink.
 */
enum class CompatibilityMode {
    Auto, ///< Use detected autopilot (default, current behavior)
    Pure, ///< Pure standard MAVLink - no autopilot-specific quirks
    Px4, ///< Force PX4 quirks regardless of detection
    ArduPilot, ///< Force ArduPilot quirks regardless of detection
};

/**
 * @brief Stream operator to print information about a `CompatibilityMode`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& os, const CompatibilityMode& mode);

} // namespace mavsdk
