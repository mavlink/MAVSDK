#pragma once

namespace mavsdk {

/**
 * @brief Autopilot type
 */
enum class Autopilot {
    Unknown, // Autopilot unknown
    Px4, // PX4
    ArduPilot, // ArduPilot
};

} // namespace mavsdk
