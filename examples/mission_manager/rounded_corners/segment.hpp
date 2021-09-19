#pragma once

#include "point.hpp"
#include <vector>

namespace rounded_corners {

class Segment {
public:
    Segment(Point a, Point b) : a(a), b(b) {}

    [[nodiscard]] double length() const;

    [[nodiscard]] Point direction() const;

    [[nodiscard]] std::vector<Point> sample(double resolution) const;

    Point a;
    Point b;
};

} // namespace rounded_corners