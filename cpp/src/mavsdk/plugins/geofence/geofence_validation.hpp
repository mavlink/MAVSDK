#pragma once

#include "plugins/geofence/geofence.hpp"
#include "mavsdk_export.h"

namespace mavsdk {

MAVSDK_TEST_EXPORT bool is_valid_geofence(const Geofence::GeofenceData& data);

} // namespace mavsdk
