#pragma once

#include "autopilot.h"
#include <functional>

namespace mavsdk {

using AutopilotCallback = std::function<Autopilot()>;

} // namespace mavsdk
