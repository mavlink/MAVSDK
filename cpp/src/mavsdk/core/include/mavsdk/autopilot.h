#pragma once

#include <sstream>
#include "mavsdk_export.h"

namespace mavsdk {

/**
 * @brief Autopilot type
 */
enum class Autopilot {
    Unknown, // Autopilot unknown
    Px4, // PX4
    ArduPilot, // ArduPilot
};

/**
 * @brief Stream operator to print information about an `Autopilot`.
 *
 * @return A reference to the stream.
 */
MAVSDK_PUBLIC std::ostream& operator<<(std::ostream& os, const Autopilot& autopilot);

} // namespace mavsdk
