#pragma once

#include "mavsdk.h"
#include <cmath>

namespace mavsdk {

class PathChecker {
public:
    void set_max_altitude(float relative_altitude_m);
    void set_min_altitude(float relative_altitude_m);
    void set_next_reach_altitude(float relative_altitude_m);
    void set_margin(float margin_m);
    void check_current_alitude(float current_altitude);

private:
    float _max_altitude_m = NAN;
    float _min_altitude_m = NAN;
    float _next_reach_altitude = NAN;
    float _last_altitude_m = NAN;
    bool _reached_altitude = false;
    float _margin_m = 2.0f; // FIXME: high margin for filter faults happening on landing
};

} // namespace mavsdk
