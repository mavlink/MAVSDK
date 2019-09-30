#include "plugins/telemetry/path_checker.h"
#include "global_include.h"

using namespace mavsdk;
using namespace mavsdk::telemetry::path_checker;

bool Line::is_on(const Point& point, float margin_m) const
{
    // TODO: to implement
    UNUSED(point);
    UNUSED(margin_m);
    return true;
}

bool TakeoffSegment::check_position_ned(const mavsdk::Telemetry::PositionNED& position_ned) const
{
    if (!_takeoff_line.is_on(Point(position_ned), _margin_m)) {
        return false;
    }

    return true;
}

bool TakeoffSegment::is_done() const
{
    // TODO: actually check
    return true;
}

void TakeoffSegment::set_takeoff_altitude(float altitude_m)
{
    auto end = _takeoff_line.get_end();
    end.set_z(-altitude_m);
    _takeoff_line.set_end(end);
}

void PathChecker::add_segment(std::unique_ptr<Segment> segment)
{
    // FIXME: Use make_unique with C++14.
    _segments.emplace_back(std::move(segment));
}

void PathChecker::set_position(const mavsdk::Telemetry::Position& position)
{
    UNUSED(position);
    // TODO: convert from global to local
    //_last_position = position;
}

void PathChecker::set_position_ned(const mavsdk::Telemetry::PositionNED& position_ned)
{
    _last_position_ned = position_ned;
}

#if 0
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
        // TODO: enable again
        // EXPECT_LT(current_altitude, _max_altitude_m + _margin_m);
    }

    if (std::isfinite(_min_altitude_m)) {
        // TODO: enable again
        // EXPECT_GT(current_altitude, _min_altitude_m - _margin_m);
    }

    if (std::isfinite(_last_altitude_m) && std::isfinite(_next_reach_altitude)) {
        if (std::fabs(current_altitude - _next_reach_altitude) < 2.0f * _margin_m) {
            _reached_altitude = true;
        }

        if (!_reached_altitude && std::fabs(_next_reach_altitude - current_altitude) > _margin_m) {
            if (_next_reach_altitude > current_altitude) {
                // TODO: enable again
                // EXPECT_GT(current_altitude, _last_altitude_m - _margin_m);
            } else {
                // TODO: enable again
                // EXPECT_LT(current_altitude, _last_altitude_m + _margin_m);
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
#endif
