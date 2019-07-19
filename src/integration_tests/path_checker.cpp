#include "path_checker.h"
#include "integration_test_helper.h"

using namespace mavsdk;

void PathChecker::set_max_altitude(float relative_altitude_m)
{
    _max_altitude_m = relative_altitude_m;
}

void PathChecker::set_min_altitude(float relative_altitude_m)
{
    _min_altitude_m = relative_altitude_m;
}

void PathChecker::set_next_reach_altitude(float relative_altitude_m)
{
    _next_reach_altitude = relative_altitude_m;
    _reached_altitude = false;
}

void PathChecker::set_margin(float margin_m)
{
    _margin_m = margin_m;
}

void PathChecker::check_current_alitude(float current_altitude)
{
    if (std::isfinite(_max_altitude_m)) {
        EXPECT_LT(current_altitude, _max_altitude_m + _margin_m);
    }

    if (std::isfinite(_min_altitude_m)) {
        EXPECT_GT(current_altitude, _min_altitude_m - _margin_m);
    }

    if (std::isfinite(_last_altitude_m) && std::isfinite(_next_reach_altitude)) {
        if (std::fabs(current_altitude - _next_reach_altitude) < 2.0f * _margin_m) {
            _reached_altitude = true;
        }

        if (!_reached_altitude && std::fabs(_next_reach_altitude - current_altitude) > _margin_m) {
            if (_next_reach_altitude > current_altitude) {
                EXPECT_GT(current_altitude, _last_altitude_m - _margin_m);
            } else {
                EXPECT_LT(current_altitude, _last_altitude_m + _margin_m);
            }
        }
    }

    if (!std::isfinite(_last_altitude_m)) {
        _last_altitude_m = current_altitude;
    } else {
        // We some filtering, trying to get rid of noise.
        _last_altitude_m = (_last_altitude_m * 0.8f) + (0.2f * current_altitude);
    }
}
