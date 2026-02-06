#include "autopilot.h"

namespace mavsdk {

std::ostream& operator<<(std::ostream& str, const Autopilot& autopilot)
{
    switch (autopilot) {
        case Autopilot::Px4:
            return str << "Px4";
        case Autopilot::ArduPilot:
            return str << "ArduPilot";
        default:
            return str << "Unknown";
    }
}

} // namespace mavsdk
