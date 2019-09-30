#pragma once

#include "plugins/telemetry/telemetry.h"
#include <cmath>
#include <memory>
#include <vector>

namespace mavsdk {
namespace telemetry {
namespace path_checker {

class Point {
public:
    Point(float x_m, float y_m, float z_m) : _x_m(x_m), _y_m(y_m), _z_m(z_m) {}

    Point(const mavsdk::Telemetry::PositionNED& position_ned) :
        _x_m(position_ned.north_m),
        _y_m(position_ned.east_m),
        _z_m(position_ned.down_m)
    {}

    void set_z(float z_m) { _z_m = z_m; }

private:
    float _x_m{NAN};
    float _y_m{NAN};
    float _z_m{NAN};
};

class Line {
public:
    Line(Point start, Point end) : _start(start), _end(end) {}

    bool is_on(const Point& point, float margin_m) const;

    Point get_end() { return _end; }
    Point get_start() { return _start; }
    void set_end(Point end) { _end = end; }
    void set_start(Point start) { _start = start; }

private:
    Point _start;
    Point _end;
};

class Segment {
public:
    virtual ~Segment() = default;
    virtual bool check_position_ned(const mavsdk::Telemetry::PositionNED& position_ned) const = 0;
    virtual bool is_done() const = 0;

protected:
    float _margin_m{0.5f};
    bool _is_done{false};
};

class TakeoffSegment : Segment {
public:
    virtual bool check_position_ned(const mavsdk::Telemetry::PositionNED& position_ned) const;
    virtual bool is_done() const;
    void set_takeoff_altitude(float altitude_m);

private:
    Line _takeoff_line{{0.f, 0.f, 0.f}, {0.f, 0.f, 2.5f}};
};

class PathChecker {
public:
    void add_segment(std::unique_ptr<Segment> segment);

    void set_position(const mavsdk::Telemetry::Position& position);
    void set_position_ned(const mavsdk::Telemetry::PositionNED& position_ned);
    // void set_max_altitude(float relative_altitude_m);
    // void set_min_altitude(float relative_altitude_m);
    // void set_next_reach_altitude(float relative_altitude_m);
    // void set_margin(float margin);
    // void check_current_alitude(float current_altitude);
    // bool position_ok();
    // bool velocity_ok();

private:
    std::vector<std::unique_ptr<Segment>> _segments{};

    mavsdk::Telemetry::PositionNED _last_position_ned{};
    // float _max_altitude_m = NAN;
    // float _min_altitude_m = NAN;
    // float _next_reach_altitude = NAN;
    // float _last_altitude_m = NAN;
    // bool _reached_altitude = false;
    // float _margin_m = 1.0f;
};

} // namespace path_checker
} // namespace telemetry
} // namespace mavsdk
