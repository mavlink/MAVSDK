#include "compatibility_mode.h"

namespace mavsdk {

std::ostream& operator<<(std::ostream& os, const CompatibilityMode& mode)
{
    switch (mode) {
        case CompatibilityMode::Auto:
            return os << "Auto";
        case CompatibilityMode::Pure:
            return os << "Pure";
        case CompatibilityMode::Px4:
            return os << "Px4";
        case CompatibilityMode::ArduPilot:
            return os << "ArduPilot";
        default:
            return os << "Unknown";
    }
}

} // namespace mavsdk
