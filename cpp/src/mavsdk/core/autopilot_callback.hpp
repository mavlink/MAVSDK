#pragma once

#include "autopilot.hpp"
#include <functional>

namespace mavsdk {

using AutopilotCallback = std::function<Autopilot()>;

} // namespace mavsdk
