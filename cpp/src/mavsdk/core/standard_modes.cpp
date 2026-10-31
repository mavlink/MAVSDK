#include "standard_modes.hpp"
#include "mavlink_include.hpp"

#include <array>
#include <utility>

namespace mavsdk {

namespace {

const std::array<std::pair<uint8_t, const char*>, 8> standard_modes{{
    {MAV_STANDARD_MODE_POSITION_HOLD, "Position"},
    {MAV_STANDARD_MODE_ORBIT, "Orbit"},
    {MAV_STANDARD_MODE_CRUISE, "Cruise"},
    {MAV_STANDARD_MODE_ALTITUDE_HOLD, "Altitude"},
    {MAV_STANDARD_MODE_SAFE_RECOVERY, "Safe Recovery"},
    {MAV_STANDARD_MODE_MISSION, "Mission"},
    {MAV_STANDARD_MODE_LAND, "Land"},
    {MAV_STANDARD_MODE_TAKEOFF, "Takeoff"},
}};

} // namespace

std::string standard_mode_name(uint8_t standard_mode)
{
    for (const auto& [mode, name] : standard_modes) {
        if (mode == standard_mode) {
            return name;
        }
    }
    return "";
}

uint8_t standard_mode_from_name(const std::string& name)
{
    for (const auto& [mode, mode_name] : standard_modes) {
        if (name == mode_name) {
            return mode;
        }
    }
    return MAV_STANDARD_MODE_NON_STANDARD;
}

} // namespace mavsdk
