#pragma once

#include <sstream>

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
std::ostream& operator<<(std::ostream& os, const Autopilot& autopilot);

} // namespace mavsdk
